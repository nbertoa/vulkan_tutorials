#include "Buffer.h"

#include <cassert>

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
Buffer::Buffer(const LogicalDevice& logicalDevice,
               const VkDeviceSize sizeInBytes,
               const VkBufferUsageFlags usageFlags,
               const VkSharingMode sharingMode)
    : mLogicalDevice(logicalDevice)
{
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // The size of the buffer in bytes.
    createInfo.size = sizeInBytes;
    // Indicate for which purposes the data in the buffer
    // is going to be used.
    createInfo.usage = usageFlags;
    // Just like the images in the swap chain, buffers can
    // also be owned by a specific queue family or be 
    // shared between multiple at the same time.
    createInfo.sharingMode = sharingMode;
    // The flags parameter is used to configure sparse
    // buffer memory, which is not relevant for us.
    createInfo.flags = 0;

    vkChecker(vkCreateBuffer(mLogicalDevice.vkDevice(),
                             &createInfo,
                             nullptr,
                             &mBuffer));
}

Buffer::~Buffer() {
    vkDestroyBuffer(mLogicalDevice.vkDevice(),
                    mBuffer,
                    nullptr);
}

Buffer::Buffer(Buffer&& other) noexcept 
    : mLogicalDevice(other.mLogicalDevice)
    , mBuffer(other.mBuffer)
{
    other.mBuffer = VK_NULL_HANDLE;
}

VkMemoryRequirements 
Buffer::memoryRequirements() const {
    assert(mBuffer != VK_NULL_HANDLE);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(mLogicalDevice.vkDevice(),
                                  mBuffer,
                                  &memoryRequirements);

    return memoryRequirements;
}

}