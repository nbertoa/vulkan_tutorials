#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Buffer.h"
#include "DeviceMemory.h"
#include "../DebugUtils.h"
#include "../command/CommandBuffer.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"
#include "../sync/Fence.h"

namespace vk {
Image::Image(const LogicalDevice& logicalDevice,
             const PhysicalDevice& physicalDevice,
             const VkImageType imageType,
             const VkExtent3D& imageExtent,
             const VkFormat format,
             const VkImageUsageFlags imageUsageFlags,
             const VkMemoryPropertyFlags memoryPropertyFlags,
             const uint32_t mipLevelCount,
             const VkImageLayout initialImageLayout,
             const VkSampleCountFlagBits sampleCount,
             const VkImageTiling imageTiling,
             const uint32_t arrayLayerCount,
             const VkSharingMode sharingMode,
             const VkImageCreateFlags flags,
             const std::vector<uint32_t> & queueFamilyIndices)
    : mLogicalDevice(logicalDevice)
    , mHasDeviceMemoryOwnership(true)
    , mDeviceMemory(new DeviceMemory(mLogicalDevice,
                                     physicalDevice,
                                     imageMemoryRequirements(),
                                     memoryPropertyFlags))
    , mExtent(imageExtent)
    , mLastLayout(initialImageLayout)
{
    VkImageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = imageType;
    createInfo.extent = imageExtent;
    createInfo.format = format;
    createInfo.usage = imageUsageFlags;
    createInfo.mipLevels = mipLevelCount;
    createInfo.initialLayout = initialImageLayout;
    createInfo.samples = sampleCount;
    createInfo.tiling = imageTiling;
    createInfo.arrayLayers = arrayLayerCount;
    createInfo.sharingMode = sharingMode;
    createInfo.flags = flags;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
    createInfo.pQueueFamilyIndices = queueFamilyIndices.empty() ? nullptr : queueFamilyIndices.data();

    vkChecker(vkCreateImage(logicalDevice.vkDevice(),
                            &createInfo,
                            nullptr,
                            &mImage));

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
Image::setImageLayout(const VkImageLayout newLayout) {
    assert(mImage != nullptr);
    assert(mLastLayout != newLayout);
    mLastLayout = newLayout;
}

void
Image::copyFromDataToDeviceMemory(void* sourceData,
                                  const VkDeviceSize size,
                                  const PhysicalDevice& physicalDevice,
                                  const CommandPool& transferCommandPool) {
    assert(sourceData != nullptr);
    assert(size > 0);

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
Image::copyFromFileToDeviceMemory(const std::string& imageFilePath,
                                  const PhysicalDevice& physicalDevice,
                                  const CommandPool& transferCommandPool) {
    int textureWidth = 0;
    int textureHeight = 0;
    int textureChannels = 0;
    stbi_uc* imageData = stbi_load(imageFilePath.c_str(),
                                   &textureWidth,
                                   &textureHeight,
                                   &textureChannels,
                                   STBI_rgb_alpha);

    assert(imageData != nullptr);

    const VkDeviceSize imageSize = static_cast<uint32_t>(textureWidth) * textureHeight * 4;

    copyFromDataToDeviceMemory(imageData,
                               imageSize,
                               physicalDevice,
                               transferCommandPool);
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
}