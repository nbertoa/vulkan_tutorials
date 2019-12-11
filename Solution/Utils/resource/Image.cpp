#include "Image.h"

#include "Buffer.h"
#include "ImageMemoryBarrier.h"
#include "../DebugUtils.h"
#include "../command/CommandBuffer.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"

namespace vk2 {
Image::Image(const uint32_t imageWidth,
             const uint32_t imageHeight,
             const VkFormat format,
             const VkImageUsageFlags imageUsageFlags,
             const vk::MemoryPropertyFlags deviceMemoryProperties,
             const uint32_t mipLevelCount,
             const vk::ImageLayout initialImageLayout,
             const VkImageType imageType,
             const VkSampleCountFlagBits sampleCount,
             const uint32_t imageDepth,
             const VkImageTiling imageTiling,
             const uint32_t arrayLayerCount,
             const VkSharingMode sharingMode,
             const VkImageCreateFlags flags,
             const std::vector<uint32_t>& queueFamilyIndices)
    : mExtent {imageWidth, imageHeight, imageDepth}
    , mLastLayout(initialImageLayout)
    , mImage(createImage(format,
                         imageUsageFlags,
                         mipLevelCount,
                         imageType,
                         sampleCount,
                         imageTiling,
                         arrayLayerCount,
                         sharingMode,
                         flags,
                         queueFamilyIndices))
    , mHasDeviceMemoryOwnership(true)
{
    const vk::MemoryRequirements memoryRequirements = LogicalDevice::device().getImageMemoryRequirements(mImage);
    vk::MemoryAllocateInfo info;
    info.setAllocationSize(memoryRequirements.size);
    info.setMemoryTypeIndex(PhysicalDevice::memoryTypeIndex(memoryRequirements.memoryTypeBits,
                                                            deviceMemoryProperties));

    mDeviceMemory = LogicalDevice::device().allocateMemory(info);

    vkChecker(vkBindImageMemory(LogicalDevice::device(),
                                mImage,
                                mDeviceMemory,
                                0));
}

Image::~Image() {
    vkDestroyImage(LogicalDevice::device(),
                   mImage,
                   nullptr);

    if (mHasDeviceMemoryOwnership) {
        LogicalDevice::device().freeMemory(mDeviceMemory);
    }
}

Image::Image(Image&& other) noexcept
    : mImage(other.mImage)
    , mDeviceMemory(other.mDeviceMemory) {
    other.mImage = VK_NULL_HANDLE;
    other.mDeviceMemory = nullptr;
}

VkImage
Image::vkImage() const {
    assert(mImage != nullptr);
    return mImage;
}

uint32_t
Image::width() const {
    assert(mImage != VK_NULL_HANDLE);
    return mExtent.width;
}

uint32_t
Image::height() const {
    assert(mImage != VK_NULL_HANDLE);
    return mExtent.height;
}

vk::ImageLayout
Image::lastImageLayout() const {
    assert(mImage != VK_NULL_HANDLE);
    return mLastLayout;
}

void
Image::copyFromDataToDeviceMemory(void* sourceData,
                                  const vk::DeviceSize size,
                                  const vk::CommandPool transferCommandPool) {
    assert(sourceData != nullptr);
    assert(size > 0);

    transitionImageLayout(vk::ImageLayout::eTransferDstOptimal,
                          transferCommandPool);

    Buffer stagingBuffer = Buffer::createAndFillStagingBuffer(sourceData,
                                                              size);

    // Fence to be signaled once
    // the copy operation is complete. 
    vk::Device device(LogicalDevice::device());
    vk::UniqueFence fence = device.createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
    vkChecker(device.waitForFences({fence.get()},
                                   VK_TRUE,
                                   std::numeric_limits<uint64_t>::max()));
    device.resetFences({fence.get()});

    CommandBuffer commandBuffer(transferCommandPool,
                                vk::CommandBufferLevel::ePrimary);
    commandBuffer.beginRecording(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    VkBufferImageCopy bufferImageCopy = {};
    bufferImageCopy.bufferOffset = 0;
    bufferImageCopy.bufferRowLength = 0;
    bufferImageCopy.bufferImageHeight = 0;
    bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferImageCopy.imageSubresource.mipLevel = 0;
    bufferImageCopy.imageSubresource.baseArrayLayer = 0;
    bufferImageCopy.imageSubresource.layerCount = 1;
    bufferImageCopy.imageOffset = {0, 0, 0};
    bufferImageCopy.imageExtent = mExtent;
    commandBuffer.copyBufferToImage(stagingBuffer,
                                    *this,
                                    bufferImageCopy);

    commandBuffer.endRecording();

    commandBuffer.submit(LogicalDevice::transferQueue(),
                         nullptr,
                         nullptr,
                         fence.get(),
                         vk::PipelineStageFlagBits::eTransfer);

    vkChecker(device.waitForFences({fence.get()},
                                   VK_TRUE,
                                   std::numeric_limits<uint64_t>::max()));
}

void
Image::transitionImageLayout(const vk::ImageLayout newImageLayout,
                             const vk::CommandPool transitionCommandPool) {
    assert(mImage != VK_NULL_HANDLE);
    assert(mLastLayout != newImageLayout);

    // Fence to be signaled once
    // the transition operation is complete. 
    vk::Device device(LogicalDevice::device());
    vk::UniqueFence fence = device.createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
    vkChecker(device.waitForFences({fence.get()},
                                   VK_TRUE,
                                   std::numeric_limits<uint64_t>::max()));
    device.resetFences({fence.get()});

    CommandBuffer commandBuffer(transitionCommandPool,
                                vk::CommandBufferLevel::ePrimary);
    commandBuffer.beginRecording(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    // Initialize to the first value in the enum
    vk::AccessFlagBits destAccessType = vk::AccessFlagBits::eIndirectCommandRead;
    vk::PipelineStageFlagBits destPipelineStages = vk::PipelineStageFlagBits::eTopOfPipe;

    // Transfer writes must occur in the pipeline transfer stage. 
    // Since the writes do not have to wait on anything,
    // you may specify an empty access mask and the earliest possible
    // pipeline stage VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT for the
    // pre-barrier operations.
    // It should be noted that VK_PIPELINE_STAGE_TRANSFER_BIT is not 
    // a real stage within the graphics and compute pipelines.
    // It is more of a pseudo-stage where transfers happen.
    if (mLastLayout == vk::ImageLayout::eUndefined &&
        newImageLayout == vk::ImageLayout::eTransferDstOptimal) {
        destAccessType = vk::AccessFlagBits::eTransferWrite;

        assert(mLastPipelineStages == vk::PipelineStageFlagBits::eTopOfPipe);
        destPipelineStages = vk::PipelineStageFlagBits::eTransfer;
    } else if (mLastLayout == vk::ImageLayout::eTransferDstOptimal &&
               newImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        // The image will be written in the same pipeline stage and
        // subsequently read by a shader, which is why 
        // we specify shader reading access in the shader
        // pipeline stage.
        assert(mLastAccessType == vk::AccessFlagBits::eTransferWrite);
        destAccessType = vk::AccessFlagBits::eShaderRead;

        assert(mLastPipelineStages == vk::PipelineStageFlagBits::eTransfer);
        destPipelineStages = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        assert(false && "Unsupported image layout transition");
    }

    ImageMemoryBarrier barrier(*this,
                               newImageLayout,
                               mLastAccessType,
                               destAccessType);

    commandBuffer.imagePipelineBarrier(barrier,
                                       mLastPipelineStages,
                                       destPipelineStages);

    commandBuffer.endRecording();

    commandBuffer.submit(LogicalDevice::transferQueue(),
                         nullptr,
                         nullptr,
                         fence.get(),
                         vk::PipelineStageFlagBits::eTransfer);

    vkChecker(device.waitForFences({fence.get()},
                                   VK_TRUE,
                                   std::numeric_limits<uint64_t>::max()));

    mLastLayout = newImageLayout;
    mLastAccessType = destAccessType;
    mLastPipelineStages = destPipelineStages;
}

VkMemoryRequirements
Image::imageMemoryRequirements() const {
    assert(mImage != VK_NULL_HANDLE);

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(LogicalDevice::device(),
                                 mImage,
                                 &memoryRequirements);

    return memoryRequirements;
}

VkImage
Image::createImage(const VkFormat format,
                   const VkImageUsageFlags imageUsageFlags,
                   const uint32_t mipLevelCount,
                   const VkImageType imageType,
                   const VkSampleCountFlagBits sampleCount,
                   const VkImageTiling imageTiling,
                   const uint32_t arrayLayerCount,
                   const VkSharingMode sharingMode,
                   const VkImageCreateFlags flags,
                   const std::vector<uint32_t>& queueFamilyIndices) {
    VkImageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = imageType;
    createInfo.extent = mExtent;
    createInfo.format = format;
    createInfo.usage = imageUsageFlags;
    createInfo.mipLevels = mipLevelCount;
    createInfo.initialLayout = (VkImageLayout)mLastLayout;
    createInfo.samples = sampleCount;
    createInfo.tiling = imageTiling;
    createInfo.arrayLayers = arrayLayerCount;
    createInfo.sharingMode = sharingMode;
    createInfo.flags = flags;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
    createInfo.pQueueFamilyIndices = queueFamilyIndices.empty() ? 
        nullptr : 
        queueFamilyIndices.data();

    VkImage image;
    vkChecker(vkCreateImage(LogicalDevice::device(),
                            &createInfo,
                            nullptr,
                            &image));

    return image;
}

}