#include "Image.h"

#include "Buffer.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"

namespace vulkan {
Image::Image(const uint32_t imageWidth,
             const uint32_t imageHeight,
             const vk::Format format,
             const vk::ImageUsageFlags imageUsageFlags,
             const vk::MemoryPropertyFlags deviceMemoryProperties,
             const uint32_t mipLevelCount,
             const vk::ImageLayout initialImageLayout,
             const vk::ImageType imageType,
             const vk::SampleCountFlagBits sampleCount,
             const uint32_t imageDepth,
             const vk::ImageTiling imageTiling,
             const uint32_t arrayLayerCount,
             const vk::SharingMode sharingMode,
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
                         queueFamilyIndices))
    , mHasDeviceMemoryOwnership(true)
{
    const vk::MemoryRequirements memoryRequirements = LogicalDevice::device().getImageMemoryRequirements(mImage);
    vk::MemoryAllocateInfo info;
    info.setAllocationSize(memoryRequirements.size);
    info.setMemoryTypeIndex(PhysicalDevice::memoryTypeIndex(memoryRequirements.memoryTypeBits,
                                                            deviceMemoryProperties));

    mDeviceMemory = LogicalDevice::device().allocateMemory(info);

    LogicalDevice::device().bindImageMemory(mImage,
                                            mDeviceMemory,
                                            0);
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
    other.mImage = vk::Image();
    other.mDeviceMemory = nullptr;
}

vk::Image
Image::vkImage() const {
    assert(mImage != VK_NULL_HANDLE);
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
    device.waitForFences({fence.get()},
                         VK_TRUE,
                         std::numeric_limits<uint64_t>::max());
    device.resetFences({fence.get()});

    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.setCommandBufferCount(1);
    allocInfo.setCommandPool(transferCommandPool);
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    vk::UniqueCommandBuffer commandBuffer = std::move(LogicalDevice::device().allocateCommandBuffersUnique(allocInfo).front());

    commandBuffer->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    vk::BufferImageCopy bufferImageCopy;
    vk::ImageSubresourceLayers imageSubresource;
    imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
    imageSubresource.setLayerCount(1);
    bufferImageCopy.setImageSubresource(imageSubresource);
    bufferImageCopy.setImageExtent(mExtent);
    commandBuffer->copyBufferToImage(stagingBuffer.vkBuffer(),
                                     mImage,
                                     vk::ImageLayout::eTransferDstOptimal,
                                     {bufferImageCopy});

    commandBuffer->end();
    
    vk::SubmitInfo info;
    info.setCommandBufferCount(1);
    info.setPCommandBuffers(&commandBuffer.get());
    const vk::PipelineStageFlags flags(vk::PipelineStageFlagBits::eTransfer);
    info.setPWaitDstStageMask(&flags);
    LogicalDevice::transferQueue().submit({info},
                                          fence.get());

    device.waitForFences({fence.get()},
                         VK_TRUE,
                         std::numeric_limits<uint64_t>::max());
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
    device.waitForFences({fence.get()},
                         VK_TRUE,
                         std::numeric_limits<uint64_t>::max());
    device.resetFences({fence.get()});

    // Command buffer for the transition
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.setCommandBufferCount(1);
    allocInfo.setCommandPool(transitionCommandPool);
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    vk::UniqueCommandBuffer commandBuffer = std::move(LogicalDevice::device().allocateCommandBuffersUnique(allocInfo).front());
    commandBuffer->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    // Initialize to the first value in the enum
    vk::AccessFlags destAccessType;
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
    } else if (mLastLayout == vk::ImageLayout::eUndefined &&
               newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        destAccessType = vk::AccessFlagBits::eDepthStencilAttachmentRead |
                         vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        destPipelineStages = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    } else {
        assert(false && "Unsupported image layout transition");
    }

    vk::ImageMemoryBarrier barrier;
    barrier.setImage(mImage);
    barrier.setOldLayout(mLastLayout);
    barrier.setNewLayout(newImageLayout);
    vk::ImageSubresourceRange imageSubresourceRange;
    if (newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
    } else {
        imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    }    
    imageSubresourceRange.setLevelCount(1);
    imageSubresourceRange.setLayerCount(1);
    barrier.setSubresourceRange(imageSubresourceRange);
    barrier.setSrcAccessMask(mLastAccessType);
    barrier.setDstAccessMask(destAccessType);

    commandBuffer->pipelineBarrier(mLastPipelineStages,
                                   destPipelineStages,
                                   vk::DependencyFlagBits::eByRegion,
                                   {}, // memory barriers
                                   {}, // buffer memory barriers
                                   {barrier});

    commandBuffer->end();

    vk::SubmitInfo info;
    info.setCommandBufferCount(1);
    info.setPCommandBuffers(&commandBuffer.get());
    const vk::PipelineStageFlags flags(vk::PipelineStageFlagBits::eTransfer);
    info.setPWaitDstStageMask(&flags);
    LogicalDevice::transferQueue().submit({info},
                                          fence.get());

    device.waitForFences({fence.get()},
                         VK_TRUE,
                         std::numeric_limits<uint64_t>::max());

    mLastLayout = newImageLayout;
    mLastAccessType = destAccessType;
    mLastPipelineStages = destPipelineStages;
}

vk::Image
Image::createImage(const vk::Format format,
                   const vk::ImageUsageFlags imageUsageFlags,
                   const uint32_t mipLevelCount,
                   const vk::ImageType imageType,
                   const vk::SampleCountFlagBits sampleCount,
                   const vk::ImageTiling imageTiling,
                   const uint32_t arrayLayerCount,
                   const vk::SharingMode sharingMode,
                   const std::vector<uint32_t>& queueFamilyIndices) {
    vk::ImageCreateInfo info = {};
    info.setImageType(imageType);
    info.setExtent(mExtent);
    info.setFormat(format);
    info.setUsage(imageUsageFlags);
    info.setMipLevels(mipLevelCount);
    info.setInitialLayout(mLastLayout);
    info.setSamples(sampleCount);
    info.setTiling(imageTiling);
    info.setArrayLayers(arrayLayerCount);
    info.setSharingMode(sharingMode);
    info.setQueueFamilyIndexCount(static_cast<uint32_t>(queueFamilyIndices.size()));
    info.setPQueueFamilyIndices(queueFamilyIndices.empty() ?
                                nullptr :
                                queueFamilyIndices.data());
                                
    return LogicalDevice::device().createImage(info);
}

}