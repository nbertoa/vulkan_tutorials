#ifndef UTILS_BUFFER
#define UTILS_BUFFER

#include <memory>
#include <vulkan/vulkan.h>

#include "DeviceMemory.h"

namespace vk {
class CommandPool;
class Fence;
class LogicalDevice;

// VkBuffer's wrapper.
// Buffers represent linear arrays of data which are 
// used for various purposes by binding them to a
// graphics or compute pipeline via descriptor sets
// or via certain commands, or by directly specifying 
// them as parameters to certain commands.
class Buffer {
public:
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
    //
    // Sharing Modes:
    // - VK_SHARING_MODE_EXCLUSIVE specifies that access 
    // to any range or image subresource of the object 
    // will be exclusive to a single queue family at a time.
    //
    // - VK_SHARING_MODE_CONCURRENT specifies that concurrent 
    // access to any range or image subresource of the object 
    // from multiple queue families is supported.
    //
    // Memory property flags:
    // - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory 
    // allocated with this type can be mapped for host access using vkMapMemory.
    //
    // - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache 
    // management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
    // are not needed to flush host writes to the device or make device writes visible 
    // to the host, respectively.
    //
    // - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit specifies that memory allocated with 
    // this type is the most efficient for device access. 
    // This property will be set if and only if the memory type belongs to a heap with 
    // the VK_MEMORY_HEAP_DEVICE_LOCAL_BIT set.
    Buffer(const LogicalDevice& logicalDevice,
           const VkDeviceSize sizeInBytes, 
           const VkBufferUsageFlags usageFlags,
           const VkSharingMode sharingMode,
           const VkMemoryPropertyFlags memoryPropertyFlags);
    ~Buffer();
    Buffer(Buffer&& other) noexcept;
    Buffer(const Buffer&) = delete;
    const Buffer& operator=(const Buffer&) = delete;  
    
    VkBuffer vkBuffer() const;

    VkDeviceSize size() const;
    
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
    // performance than explicit flushing.
    //
    // Flushing memory ranges or using a coherent memory heap
    // means that the driver will be aware of our writes to
    // the buffer, but it does not mean that they are actually
    // visible on the GPU yet. The transfer of data to the GPU
    // is an operation that happens in the background and the
    // specification simply tells us that it is guaranteed
    // to be complete as of the next call to vkQueueSubmit.
    //
    // Preconditions:
    // This method cannot be used if you allocated the buffer
    // from a memory type that is device local, like 
    // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT.
    // You should use methods like copyFromBufferToDeviceMemory instead.
    void copyToHostMemory(void* sourceData, 
                          const VkDeviceSize offset,
                          const VkDeviceSize size,
                          const VkMemoryMapFlags flags);

    // These methods assumes the buffer was created with
    // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT.
    // The methods without size parameter are going to use
    // the sourceBuffer size.
    // The methods without the fence parameter, will create
    // an internal fence and wait for completion.
    void copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                      const VkDeviceSize size,
                                      const CommandPool& transferCommandPool,
                                      const Fence& fence);
    void copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                      const VkDeviceSize size,
                                      const CommandPool& transferCommandPool);
    void copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                      const CommandPool& transferCommandPool,
                                      const Fence& fence);
    void copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                      const CommandPool& transferCommandPool);

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