#ifndef UTILS_BUFFER
#define UTILS_BUFFER

#include <cassert>
#include <memory>
#include <vulkan/vulkan.h>

#include "DeviceMemory.h"

namespace vk {
class LogicalDevice;

// VkBuffer's wrapper to be able to create/destroy
// and handle it easily.
// Buffers represent linear arrays of data which are 
// used for various purposes by binding them to a
// graphics or compute pipeline via descriptor sets
// or via certain commands, or by directly specifying 
// them as parameters to certain commands.
class Buffer {
public:
    Buffer(const LogicalDevice& logicalDevice,
           const VkDeviceSize sizeInBytes, 
           const VkBufferUsageFlags usageFlags,
           const VkSharingMode sharingMode,
           const VkMemoryPropertyFlags memoryPropertyFlags);
    ~Buffer();

    Buffer(Buffer&& other) noexcept;

    Buffer(const Buffer&) = delete;
    const Buffer& operator=(const Buffer&) = delete;  
    
    VkBuffer vkBuffer() const {
        assert(mBuffer != VK_NULL_HANDLE);
        return mBuffer;
    }
    
    // The driver may not immediately copy the data
    // into the buffer memory, for example because
    // of caching.
    // It is also possible that writes to the buffer
    // are not visible in the mapped memory yet.
    // There are two ways to deal with that problem:
    // - Use a memory heap that is host coherent,
    // indicated with VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    // - Call vkFlushMappedMemoryRanges to after writing
    // to the mapped memory, and call vkInvalidateMappedMemoryRanges
    // before reading from the mapped memory.
    //
    // The first approach ensures that the mapped memory
    // always matches the contents of the allocated memory.
    // Do keep in mind that this may lead to slightly worse
    // performance thatn explicit flushing.
    //
    // Flushing memory ranges or using a coherent memory heap
    // means that the driver will be aware of our writes to
    // the buffer, but it does not mean that they are actually
    // visible on the GPU yet. The transfer of data to the GPU
    // is an operation that happens in the background and the
    // specification simply tells us that it is guaranteed
    // to be complete as of the next call to vkQueueSubmit.
    void copyToMemory(void* sourceData, 
                      const VkDeviceSize offset,
                      const VkDeviceSize size,
                      const VkMemoryMapFlags flags);

    VkDeviceSize size() const {
        assert(mBuffer != VK_NULL_HANDLE);
        return mSizeInBytes;
    }

private:
    VkMemoryRequirements memoryRequirements() const;

    static VkBuffer createBuffer(const LogicalDevice& logicalDevice,
                                 const VkDeviceSize sizeInBytes,
                                 const VkBufferUsageFlags usageFlags,
                                 const VkSharingMode sharingMode);

    const LogicalDevice& mLogicalDevice;    
    VkBuffer mBuffer = VK_NULL_HANDLE;
    const VkDeviceSize mSizeInBytes = 0;
    DeviceMemory mDeviceMemory;
};
}

#endif 