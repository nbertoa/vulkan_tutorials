#include "Buffer.h"

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
Buffer::Buffer(const LogicalDevice& logicalDevice,
               const VkDeviceSize sizeInBytes,
               const VkBufferUsageFlags usageFlags,
               const VkSharingMode sharingMode,
               const VkMemoryPropertyFlags memoryPropertyFlags)
    : mLogicalDevice(logicalDevice)
    , mBuffer(createBuffer(logicalDevice,
                           sizeInBytes,
                           usageFlags,
                           sharingMode))
    , mSizeInBytes(sizeInBytes)
    , mDeviceMemory(mLogicalDevice,
                    memoryRequirements(),
                    memoryPropertyFlags)
{
    assert(mSizeInBytes > 0);

    // The fourth parameter is the offset within the region
    // of memory. If the offset is non-zero, then it
    // is required to be divisible by memory requirements
    // aligment field.
    vkChecker(vkBindBufferMemory(mLogicalDevice.vkDevice(),
                                 mBuffer,
                                 mDeviceMemory.vkDeviceMemory(),
                                 0));
}

Buffer::~Buffer() {
    vkDestroyBuffer(mLogicalDevice.vkDevice(),
                    mBuffer,
                    nullptr);
}

Buffer::Buffer(Buffer&& other) noexcept 
    : mLogicalDevice(other.mLogicalDevice)
    , mBuffer(other.mBuffer)
    , mDeviceMemory(std::move(other.mDeviceMemory))
{
    other.mBuffer = VK_NULL_HANDLE;
}

void 
Buffer::copyToHostMemory(void* sourceData,
                         const VkDeviceSize offset,
                         const VkDeviceSize size,
                         const VkMemoryMapFlags flags) {
    assert(mBuffer != VK_NULL_HANDLE);
    assert(sourceData != nullptr);
    assert(size > 0);

    void* destinationData;
    vkChecker(vkMapMemory(mLogicalDevice.vkDevice(),
                          mDeviceMemory.vkDeviceMemory(),
                          offset,
                          size,
                          flags,
                          &destinationData));

    memcpy(destinationData,
           sourceData,
           static_cast<size_t>(size));

    vkUnmapMemory(mLogicalDevice.vkDevice(),
                  mDeviceMemory.vkDeviceMemory());
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

VkBuffer
Buffer::createBuffer(const LogicalDevice& logicalDevice,
                     const VkDeviceSize sizeInBytes,
                     const VkBufferUsageFlags usageFlags,
                     const VkSharingMode sharingMode) {
    // Usage flags:
    // - VK_BUFFER_USAGE_VERTEX_BUFFER_BIT specifies that 
    // the buffer is suitable for passing as an element 
    // of the pBuffers array to vkCmdBindVertexBuffers.
    //
    // - VK_BUFFER_USAGE_INDEX_BUFFER_BIT specifies that 
    // the buffer is suitable for passing as the buffer 
    // parameter to vkCmdBindIndexBuffer.
    //
    // - VK_BUFFER_USAGE_TRANSFER_SRC_BIT specifies that 
    // the buffer can be used as the source of a transfer 
    // command (see the definition of VK_PIPELINE_STAGE_TRANSFER_BIT).
    //
    // - VK_BUFFER_USAGE_TRANSFER_DST_BIT specifies that 
    // the buffer can be used as the destination of 
    // a transfer command.

    // Sharing Modes:
    // - VK_SHARING_MODE_EXCLUSIVE specifies that access 
    // to any range or image subresource of the object 
    // will be exclusive to a single queue family at a time.
    //
    // - VK_SHARING_MODE_CONCURRENT specifies that concurrent 
    // access to any range or image subresource of the object 
    // from multiple queue families is supported.

    VkBuffer buffer;

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

    vkChecker(vkCreateBuffer(logicalDevice.vkDevice(),
                             &createInfo,
                             nullptr,
                             &buffer));

    assert(buffer != VK_NULL_HANDLE);

    return buffer;
}

}