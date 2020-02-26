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
    , mSrcLayout(initialImageLayout)
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
    , mSrcLayout(other.mSrcLayout)
    , mSrcAccesses(other.mSrcAccesses)
    , mSrcPipelineStages(other.mSrcPipelineStages)
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
    return mSrcLayout;
}

void
Image::copyFromDataToDeviceMemory(void* sourceData,
                                  const vk::DeviceSize size) {
    assert(sourceData != nullptr);
    assert(size > 0);

    transitionImageLayout(vk::ImageLayout::eTransferDstOptimal);    

    Buffer stagingBuffer = Buffer::createAndFillStagingBuffer(sourceData,
                                                              size);
    
    vk::ImageSubresourceLayers layer;
    layer.setAspectMask(vk::ImageAspectFlagBits::eColor);
    layer.setLayerCount(1);

    vk::BufferImageCopy bufferImageCopy;
    bufferImageCopy.setImageSubresource(layer);
    bufferImageCopy.setImageExtent(mExtent);

    vk::UniqueCommandBuffer commandBuffer = CommandPools::beginOneTimeSubmitCommandBuffer();
    commandBuffer->copyBufferToImage(stagingBuffer.vkBuffer(),
                                     mImage,
                                     vk::ImageLayout::eTransferDstOptimal,
                                     {bufferImageCopy});
    CommandPools::endAndWaitOneTimeSubmitCommandBuffer(commandBuffer.get());

    generateMipmaps();
}

void
Image::transitionImageLayout(const vk::ImageLayout destLayout) {
    assert(mImage != VK_NULL_HANDLE);
    assert(mSrcLayout != destLayout);

    // Initialize to the first value in the enum
    vk::AccessFlags destAccesses;
    vk::PipelineStageFlags destPipelineStages;

    // Transfer writes must occur in the pipeline transfer stage. 
    // Since the writes do not have to wait on anything,
    // you may specify an empty access mask and the earliest possible
    // pipeline stage VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT for the
    // pre-barrier operations.
    // It should be noted that VK_PIPELINE_STAGE_TRANSFER_BIT is not 
    // a real stage within the graphics and compute pipelines.
    // It is more of a pseudo-stage where transfers happen.
    if (mSrcLayout == vk::ImageLayout::eUndefined &&
        destLayout == vk::ImageLayout::eTransferDstOptimal) {
        destPipelineStages = vk::PipelineStageFlagBits::eTransfer;
        destAccesses = vk::AccessFlagBits::eTransferWrite;
    } else if (mSrcLayout == vk::ImageLayout::eTransferDstOptimal &&
               destLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        destPipelineStages = vk::PipelineStageFlagBits::eFragmentShader;
        destAccesses = vk::AccessFlagBits::eShaderRead;
    } else if (mSrcLayout == vk::ImageLayout::eUndefined &&
               destLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        destPipelineStages = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        destAccesses = vk::AccessFlagBits::eDepthStencilAttachmentRead |
                       vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    } else {
        assert(false && "Unsupported image layout transition");
    }

    vk::ImageSubresourceRange range;
    if (destLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        range.setAspectMask(vk::ImageAspectFlagBits::eDepth);
    } else {
        range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    }
    range.setLevelCount(mMipLevelCount);
    range.setLayerCount(1);

    vk::ImageMemoryBarrier barrier;
    barrier.setImage(mImage);
    barrier.setOldLayout(mSrcLayout);
    barrier.setNewLayout(destLayout);
    barrier.setSrcAccessMask(mSrcAccesses);
    barrier.setDstAccessMask(destAccesses);
    barrier.setSubresourceRange(range);
    
    vk::UniqueCommandBuffer commandBuffer = CommandPools::beginOneTimeSubmitCommandBuffer();
    commandBuffer->pipelineBarrier(mSrcPipelineStages,
                                   destPipelineStages,
                                   vk::DependencyFlagBits::eByRegion,
                                   {}, // memory barriers
                                   {}, // buffer memory barriers
                                   {barrier});
    CommandPools::endAndWaitOneTimeSubmitCommandBuffer(commandBuffer.get());

    mSrcLayout = destLayout;
    mSrcAccesses = destAccesses;
    mSrcPipelineStages = destPipelineStages;
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
    info.setInitialLayout(mSrcLayout);
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

void
Image::generateMipmaps() {
    assert(mImage != VK_NULL_HANDLE);

    if (mMipLevelCount == 1) {
        return;
    }

    vk::ImageSubresourceRange range;
    range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    range.setBaseArrayLayer(0);
    range.setLayerCount(1);
    range.setLevelCount(1);

    int32_t previousMipMapWidth = mExtent.width;
    int32_t previousMipMapHeight = mExtent.height;

    vk::UniqueCommandBuffer commandBuffer = CommandPools::beginOneTimeSubmitCommandBuffer();
    for (uint32_t i = 1; i < mMipLevelCount; ++i) {
        // We set the previous mipmap as the transfer source that will be read, 
        // because we are going to write to the current mip map.
        {
            range.setBaseMipLevel(i - 1);

            vk::ImageMemoryBarrier barrier;
            barrier.setImage(mImage);
            barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
            barrier.setNewLayout(vk::ImageLayout::eTransferSrcOptimal);
            barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
            barrier.setDstAccessMask(vk::AccessFlagBits::eTransferRead);
            barrier.setSubresourceRange(range);
                        
            commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                           vk::PipelineStageFlagBits::eTransfer,
                                           vk::DependencyFlagBits(),
                                           {},
                                           {},
                                           {barrier});
        }

        // Blit previous mip map to current mip map
        {
            std::array<vk::Offset3D, 2> srcOffsets;
            srcOffsets[0] = {0, 0, 0};
            srcOffsets[1] = {previousMipMapWidth, previousMipMapHeight, 1};

            std::array<vk::Offset3D, 2> destOffsets;
            destOffsets[0] = {0, 0, 0};
            destOffsets[1] = {previousMipMapWidth > 1 ? previousMipMapWidth / 2 : 1,
                              previousMipMapHeight > 1 ? previousMipMapHeight / 2 : 1,
                              1};

            vk::ImageSubresourceLayers srcLayer;
            srcLayer.setAspectMask(vk::ImageAspectFlagBits::eColor);
            srcLayer.setMipLevel(i - 1);
            srcLayer.setBaseArrayLayer(0);
            srcLayer.setLayerCount(1);

            vk::ImageSubresourceLayers destLayer;
            destLayer.setAspectMask(vk::ImageAspectFlagBits::eColor);
            destLayer.setMipLevel(i);
            destLayer.setBaseArrayLayer(0);
            destLayer.setLayerCount(1);

            vk::ImageBlit blit;
            blit.setSrcOffsets(srcOffsets);
            blit.setDstOffsets(destOffsets);
            blit.setSrcSubresource(srcLayer);
            blit.setDstSubresource(destLayer);

            commandBuffer->blitImage(mImage,
                                     vk::ImageLayout::eTransferSrcOptimal,
                                     mImage,
                                     vk::ImageLayout::eTransferDstOptimal,
                                     {blit},
                                     vk::Filter::eLinear);
        }

        // Now, set the previous mip map to be read by the fragment shader.
        {
            vk::ImageMemoryBarrier barrier;
            barrier.setImage(mImage);
            barrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal);
            barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
            barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
            barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
            barrier.setSubresourceRange(range);

            commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                           vk::PipelineStageFlagBits::eFragmentShader,
                                           vk::DependencyFlags(),
                                           {},
                                           {},
                                           {barrier});
        }

        // Update previous mip map dimensions
        if (previousMipMapWidth > 1) {
            previousMipMapWidth /= 2;
        }

        if (previousMipMapHeight > 1) {
            previousMipMapHeight /= 2;
        }
    }

    // Now, set the last mip map to be read by the fragment shader.
    {
        range.setBaseMipLevel(mMipLevelCount - 1);
        vk::ImageMemoryBarrier barrier;
        barrier.setImage(mImage);
        barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
        barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        barrier.setSubresourceRange(range);

        commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                       vk::PipelineStageFlagBits::eFragmentShader,
                                       vk::DependencyFlags(),
                                       {}, 
                                       {}, 
                                       {barrier});
    }    

    CommandPools::endAndWaitOneTimeSubmitCommandBuffer(commandBuffer.get());
        
    mSrcLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    mSrcAccesses = vk::AccessFlagBits::eShaderRead;
    mSrcPipelineStages = vk::PipelineStageFlagBits::eFragmentShader;
}

}