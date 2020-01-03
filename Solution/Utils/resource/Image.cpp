#include "Image.h"

#include "Buffer.h"
#include "../CommandPools.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"

namespace vulkan {
Image::Image(const uint32_t imageWidth,
             const uint32_t imageHeight,
             const vk::Format format,
             const vk::ImageUsageFlags imageUsageFlags,
             const vk::MemoryPropertyFlags deviceMemoryProperties,
             const vk::ImageLayout initialImageLayout,
             const vk::ImageType imageType,
             const vk::SampleCountFlagBits sampleCount,
             const uint32_t imageDepth,
             const vk::ImageTiling imageTiling,
             const uint32_t arrayLayerCount,
             const vk::SharingMode sharingMode,
             const std::vector<uint32_t>& queueFamilyIndices)
    : mExtent {imageWidth, imageHeight, imageDepth}
    , mFormat(format)
    , mLastLayout(initialImageLayout)
    , mImage(createImage(imageUsageFlags,
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
    : mExtent(other.mExtent)
    , mMipLevelCount(other.mMipLevelCount)
    , mLastLayout(other.mLastLayout)
    , mLastAccessType(other.mLastAccessType)
    , mLastPipelineStages(other.mLastPipelineStages)
    , mImage(other.mImage)
    , mHasDeviceMemoryOwnership(other.mHasDeviceMemoryOwnership)
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

uint32_t
Image::mipLevelCount() const {
    assert(mImage != VK_NULL_HANDLE);
    return mMipLevelCount;
}

vk::ImageLayout
Image::lastImageLayout() const {
    assert(mImage != VK_NULL_HANDLE);
    return mLastLayout;
}

void
Image::copyFromDataToDeviceMemory(void* sourceData,
                                  const vk::DeviceSize size) {
    assert(sourceData != nullptr);
    assert(size > 0);

    transitionImageLayout(vk::ImageLayout::eTransferDstOptimal);

    Buffer stagingBuffer = Buffer::createAndFillStagingBuffer(sourceData,
                                                              size);

    vk::UniqueCommandBuffer commandBuffer = CommandPools::beginOneTimeSubmitCommandBuffer();

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

    CommandPools::endAndWaitOneTimeSubmitCommandBuffer(commandBuffer.get());
}

void
Image::transitionImageLayout(const vk::ImageLayout newImageLayout) {
    assert(mImage != VK_NULL_HANDLE);
    assert(mLastLayout != newImageLayout);

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
    imageSubresourceRange.setLevelCount(mMipLevelCount);
    imageSubresourceRange.setLayerCount(1);
    
    barrier.setSrcAccessMask(mLastAccessType);
    barrier.setDstAccessMask(destAccessType);
    barrier.setSubresourceRange(imageSubresourceRange);
    
    // Command buffer for the transition
    vk::UniqueCommandBuffer commandBuffer = CommandPools::beginOneTimeSubmitCommandBuffer();

    commandBuffer->pipelineBarrier(mLastPipelineStages,
                                   destPipelineStages,
                                   vk::DependencyFlagBits::eByRegion,
                                   {}, // memory barriers
                                   {}, // buffer memory barriers
                                   {barrier});

    CommandPools::endAndWaitOneTimeSubmitCommandBuffer(commandBuffer.get());

    mLastLayout = newImageLayout;
    mLastAccessType = destAccessType;
    mLastPipelineStages = destPipelineStages;
}

vk::UniqueImageView
Image::createImageView(const vk::ImageAspectFlags aspectFlags) const {
    assert(mImage != VK_NULL_HANDLE);

    vk::ImageViewCreateInfo info;
    info.setImage(mImage);
    info.setFormat(mFormat);
    info.setSubresourceRange(vk::ImageSubresourceRange {aspectFlags, 0, mMipLevelCount, 0, 1});
    info.setViewType(vk::ImageViewType::e2D);
    return LogicalDevice::device().createImageViewUnique(info);
}

vk::Image
Image::createImage(const vk::ImageUsageFlags imageUsageFlags,
                   const vk::ImageType imageType,
                   const vk::SampleCountFlagBits sampleCount,
                   const vk::ImageTiling imageTiling,
                   const uint32_t arrayLayerCount,
                   const vk::SharingMode sharingMode,
                   const std::vector<uint32_t>& queueFamilyIndices) {

    // Frame buffers or depth buffer must have a single mip level.
    if (imageUsageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment ||
        imageUsageFlags & vk::ImageUsageFlagBits::eColorAttachment) {
        mMipLevelCount = 1;
    } else {
        mMipLevelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(mExtent.width, 
                                                                             mExtent.height)))) + 1;
    }

    vk::ImageCreateInfo info = {};
    info.setImageType(imageType);
    info.setExtent(mExtent);
    info.setFormat(mFormat);
    info.setUsage(imageUsageFlags);
    info.setMipLevels(mMipLevelCount);
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