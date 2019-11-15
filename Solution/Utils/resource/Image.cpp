#include "Image.h"

#include "Buffer.h"
#include "DeviceMemory.h"
#include "ImageMemoryBarrier.h"
#include "../DebugUtils.h"
#include "../command/CommandBuffer.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"
#include "../sync/Fence.h"

namespace vk {
Image::Image(const LogicalDevice& logicalDevice,
             const PhysicalDevice& physicalDevice,
             const uint32_t imageWidth,
             const uint32_t imageHeight,
             const VkFormat format,
             const VkImageUsageFlags imageUsageFlags,
             const VkMemoryPropertyFlags memoryPropertyFlags,
             const uint32_t mipLevelCount,
             const VkImageLayout initialImageLayout,
             const VkImageType imageType,
             const VkSampleCountFlagBits sampleCount,
             const uint32_t imageDepth,
             const VkImageTiling imageTiling,
             const uint32_t arrayLayerCount,
             const VkSharingMode sharingMode,
             const VkImageCreateFlags flags,
             const std::vector<uint32_t> & queueFamilyIndices)
    : mLogicalDevice(logicalDevice)
    , mExtent {imageWidth, imageHeight, imageDepth}
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
    , mDeviceMemory(new DeviceMemory(mLogicalDevice,
                                     physicalDevice,
                                     imageMemoryRequirements(),
                                     memoryPropertyFlags))
{
    // - device is the logical device that owns the buffer and memory.
    // - buffer to be attached to memory.
    // - memory is the device memory to attach.
    // - memoryOffset is the start offset of the region of memory which is to be bound to the buffer.
    //   The number of bytes returned in the VkMemoryRequirements::size member in memory, 
    //   starting from memoryOffset bytes, will be bound to the specified buffer.
    //   If the offset is non-zero, then it is required to be divisible by memory requirements
    //   aligment field.
    vkChecker(vkBindImageMemory(mLogicalDevice.vkDevice(),
                                mImage,
                                mDeviceMemory->vkDeviceMemory(),
                                0));
}

Image::~Image() {
    vkDestroyImage(mLogicalDevice.vkDevice(),
                   mImage,
                   nullptr);

    if (mHasDeviceMemoryOwnership) {
        delete mDeviceMemory;
    }
}

Image::Image(Image&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mImage(other.mImage)
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
    assert(mImage != nullptr);
    return mExtent.width;
}

uint32_t
Image::height() const {
    assert(mImage != nullptr);
    return mExtent.height;
}

VkImageLayout
Image::lastImageLayout() const {
    assert(mImage != nullptr);
    return mLastLayout;
}

void
Image::copyFromDataToDeviceMemory(void* sourceData,
                                  const VkDeviceSize size,
                                  const PhysicalDevice& physicalDevice,
                                  const CommandPool& transferCommandPool) {
    assert(sourceData != nullptr);
    assert(size > 0);

    transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          transferCommandPool);

    Buffer stagingBuffer = Buffer::createAndFillStagingBuffer(sourceData,
                                                              size,
                                                              physicalDevice,
                                                              mLogicalDevice);

    // Fence to be signaled once
    // the copy operation is complete. 
    Fence executionCompletedFence(mLogicalDevice);
    executionCompletedFence.waitAndReset();

    CommandBuffer commandBuffer = CommandBuffer::createAndBeginOneTimeSubmitCommandBuffer(mLogicalDevice,
                                                                                          transferCommandPool);

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

    commandBuffer.submit(mLogicalDevice.transferQueue(),
                         nullptr,
                         nullptr,
                         executionCompletedFence,
                         VK_PIPELINE_STAGE_TRANSFER_BIT);

    executionCompletedFence.wait();
}

void
Image::transitionImageLayout(const VkImageLayout newImageLayout,
                             const CommandPool& transitionCommandPool) {
    assert(mImage != VK_NULL_HANDLE);
    assert(mLastLayout != newImageLayout);

    // Fence to be signaled once
    // the transition operation is complete. 
    Fence executionCompletedFence(mLogicalDevice);
    executionCompletedFence.waitAndReset();

    CommandBuffer commandBuffer = CommandBuffer::createAndBeginOneTimeSubmitCommandBuffer(mLogicalDevice,
                                                                                          transitionCommandPool);

    VkAccessFlags destAccessType = 0;
    VkPipelineStageFlags destPipelineStages = 0;

    // Transfer writes must occur in the pipeline transfer stage. 
    // Since the writes do not have to wait on anything,
    // you may specify an empty access mask and the earliest possible
    // pipeline stage VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT for the
    // pre-barrier operations.
    // It should be noted that VK_PIPELINE_STAGE_TRANSFER_BIT is not 
    // a real stage within the graphics and compute pipelines.
    // It is more of a pseudo-stage where transfers happen.
    if (mLastLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        assert(mLastAccessType == 0);
        destAccessType = VK_ACCESS_TRANSFER_WRITE_BIT;

        assert(mLastPipelineStages == VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
        destPipelineStages = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (mLastLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        // The image will be written in the same pipeline stage and
        // subsequently read by a shader, which is why 
        // we specify shader reading access in the shader
        // pipeline stage.
        assert(mLastAccessType == VK_ACCESS_TRANSFER_WRITE_BIT);
        destAccessType = VK_ACCESS_SHADER_READ_BIT;

        assert(mLastPipelineStages == VK_PIPELINE_STAGE_TRANSFER_BIT);
        destPipelineStages = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
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

    commandBuffer.submit(mLogicalDevice.transferQueue(),
                         nullptr,
                         nullptr,
                         executionCompletedFence,
                         VK_PIPELINE_STAGE_TRANSFER_BIT);

    executionCompletedFence.wait();

    mLastLayout = newImageLayout;
    mLastAccessType = destAccessType;
    mLastPipelineStages = destPipelineStages;
}

VkMemoryRequirements
Image::imageMemoryRequirements() const {
    assert(mImage != VK_NULL_HANDLE);

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(mLogicalDevice.vkDevice(),
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
    createInfo.initialLayout = mLastLayout;
    createInfo.samples = sampleCount;
    createInfo.tiling = imageTiling;
    createInfo.arrayLayers = arrayLayerCount;
    createInfo.sharingMode = sharingMode;
    createInfo.flags = flags;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
    createInfo.pQueueFamilyIndices = queueFamilyIndices.empty() ? nullptr : queueFamilyIndices.data();

    VkImage image;
    vkChecker(vkCreateImage(mLogicalDevice.vkDevice(),
                            &createInfo,
                            nullptr,
                            &image));

    return image;
}

}