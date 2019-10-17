#ifndef UTILS_BUFFER
#define UTILS_BUFFER

#include <memory>
#include <vulkan/vulkan.h>

#include "DeviceMemory.h"

namespace vk {
class CommandPool;
class Fence;
class LogicalDevice;
class PhysicalDevice;

//
// VkBuffer wrapper.
//
// A buffer is a container for any binary data that just 
// has its length, expressed in bytes.
//
// They represent linear arrays of data which are 
// used for various purposes by binding them to a
// graphics or compute pipeline via descriptor sets
// or via certain commands, or by directly specifying 
// them as parameters to certain commands.
//
// Creating a Buffer of certain length does not automatically allocate memory for it. 
// It is a 3-step process that must be manually performed by you. 
// You can also choose to use our Vulkan Memory Allocator library 
// which takes care of the allocation for you
// - 1. Allocate DeviceMemory,
// - 2. Create Buffer,
// - 3. Bind them together using function vkBindBufferMemory or vkBindImageMemory.
//
// That is why you must also create a DeviceMemory object. 
// You should not allocate separate DeviceMemory for each Buffer. 
// Instead, you should allocate bigger chunks of memory and assign parts of them to your Buffer. 
// Allocation is a costly operation and there is a limit on maximum number of allocations as well, 
// all of which can be queried from your PhysicalDevice.
//
// You need the Buffer to:
// - Create the BufferView.
//
// To create/use the Buffer you need:
// - DeviceMemory for vkBindBufferMemory
//
class Buffer {
public:
    // This constructor must be used if you want that this buffer also creates
    // its own DeviceMemory.
    //
    // * logicalDevice owns the buffer and the device memory
    //
    // * physicalDevice is used to create the DeviceMemory
    //
    // * size is the size in bytes of the buffer to be created.
    //
    // * usage is a bitmask of VkBufferUsageFlagBits specifying allowed usages of the buffer:
    //
    //   - VK_BUFFER_USAGE_TRANSFER_SRC_BIT specifies that the buffer can be used 
    //     as the source of a transfer command(see the definition of VK_PIPELINE_STAGE_TRANSFER_BIT).
    //   - VK_BUFFER_USAGE_TRANSFER_DST_BIT specifies that the buffer can be used as 
    //     the destination of a transfer command.
    //   - VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT specifies that the buffer can be used 
    //     to create a VkBufferView suitable for occupying a VkDescriptorSet slot of type 
    //     VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER.
    //   - VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT specifies that the buffer can be used 
    //     to create a VkBufferView suitable for occupying a VkDescriptorSet slot of type 
    //     VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER.
    //   - VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT specifies that the buffer can be used in a 
    //     VkDescriptorBufferInfo suitable for occupying a VkDescriptorSet slot either of 
    //     type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC.
    //   - VK_BUFFER_USAGE_STORAGE_BUFFER_BIT specifies that the buffer can be used in a 
    //     VkDescriptorBufferInfo suitable for occupying a VkDescriptorSet slot either of type 
    //     VK_DESCRIPTOR_TYPE_STORAGE_BUFFER or VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC.
    //   - VK_BUFFER_USAGE_INDEX_BUFFER_BIT specifies that the buffer is suitable for passing 
    //     as the buffer parameter to vkCmdBindIndexBuffer.
    //   - VK_BUFFER_USAGE_VERTEX_BUFFER_BIT specifies that the buffer is suitable for passing 
    //     as an element of the pBuffers array to vkCmdBindVertexBuffers.
    //   - VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT specifies that the buffer is suitable for passing 
    //     as the buffer parameter to vkCmdDrawIndirect, vkCmdDrawIndexedIndirect, 
    //     vkCmdDrawMeshTasksIndirectNV, vkCmdDrawMeshTasksIndirectCountNV, or vkCmdDispatchIndirect.
    //     It is also suitable for passing as the buffer member of VkIndirectCommandsTokenNVX, or 
    //     sequencesCountBuffer or sequencesIndexBuffer member of VkCmdProcessCommandsInfoNVX
    //   - VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT specifies that the buffer is suitable for 
    //     passing as the buffer parameter to vkCmdBeginConditionalRenderingEXT.
    //   - VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT specifies that the buffer is suitable 
    //     for using for binding as a transform feedback buffer with vkCmdBindTransformFeedbackBuffersEXT.
    //   - VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT specifies that the buffer is 
    //     suitable for using as a counter buffer with vkCmdBeginTransformFeedbackEXTand vkCmdEndTransformFeedbackEXT.
    //   - VK_BUFFER_USAGE_RAY_TRACING_BIT_NV specifies that the buffer is suitable for use 
    //     in vkCmdTraceRaysNVand vkCmdBuildAccelerationStructureNV.
    //   - VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT specifies that the buffer can be 
    //     used to retrieve a buffer device address via vkGetBufferDeviceAddressEXTand use 
    //     that address to access the buffer’s memory from a shader.
    //
    // * sharingMode is a VkSharingMode value specifying the sharing mode of the buffer
    //   when it will be accessed by multiple queue families:
    //
    //   - VK_SHARING_MODE_EXCLUSIVE specifies that access 
    //     to any range or image subresource of the object 
    //     will be exclusive to a single queue family at a time.
    //   - VK_SHARING_MODE_CONCURRENT specifies that concurrent 
    //     access to any range or image subresource of the object 
    //     from multiple queue families is supported.
    //
    // * memoryPropertyFlags is used to create the DeviceMemory:
    //
    //   - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit specifies that memory allocated with 
    //     this type is the most efficient for device access.
    //     This property will be set ifand only if the memory type belongs to a heap 
    //     with the VK_MEMORY_HEAP_DEVICE_LOCAL_BIT set.
    //   - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with 
    //     this type can be mapped for host access using vkMapMemory.
    //   - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache 
    //     management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
    //     are not needed to flush host writes to the device or make device writes visible 
    //     to the host, respectively.
    //   - VK_MEMORY_PROPERTY_HOST_CACHED_BIT bit specifies that memory allocated with this 
    //     type is cached on the host.Host memory accesses to uncached memory are slower than 
    //     to cached memory, however uncached memory is always host coherent.
    //   - VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit specifies that the memory type only 
    //     allows device access to the memory.Memory types must not have both 
    //     K_MEMORY_PROPERTY_LAZILY_ALLOCATED_BITand VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set.
    //     Additionally, the object’s backing memory may be provided by the implementation lazily 
    //     as specified in Lazily Allocated Memory.
    //   - VK_MEMORY_PROPERTY_PROTECTED_BIT bit specifies that the memory type only allows device 
    //     access to the memory, and allows protected queue operations to access the memory.
    //     Memory types must not have VK_MEMORY_PROPERTY_PROTECTED_BIT setand any of 
    //     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set, or VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, 
    //     or VK_MEMORY_PROPERTY_HOST_CACHED_BIT set.
    //   - VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD bit specifies that device accesses to 
    //     allocations of this memory type are automatically made availableand visible.
    //   - VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD bit specifies that memory allocated with 
    //     this type is not cached on the device.Uncached device memory is always device coherent.
    Buffer(const LogicalDevice& logicalDevice,
           const PhysicalDevice& physicalDevice,
           const VkDeviceSize size, 
           const VkBufferUsageFlags usageFlags,
           const VkSharingMode sharingMode,
           const VkMemoryPropertyFlags memoryPropertyFlags);

    // This constructor must be used if you want to provide
    // the DeviceMemory that the buffer should use.
    // Check the first constructor for an explanation of each parameter.
    Buffer(const LogicalDevice& logicalDevice,
           const VkDeviceSize size,
           const VkBufferUsageFlags usageFlags,
           const VkSharingMode sharingMode,
           const DeviceMemory& deviceMemory);
    ~Buffer();
    Buffer(Buffer&& other) noexcept;
    Buffer(const Buffer&) = delete;
    const Buffer& operator=(const Buffer&) = delete;  
    
    VkBuffer 
    vkBuffer() const;

    VkDeviceSize 
    size() const;
    
    // The driver may not immediately copy the data
    // into the buffer memory, for example because
    // of caching.
    // It is also possible that writes to the buffer
    // are not visible in the mapped memory yet.
    //
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
    // from a memory type that is device local like , like 
    // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT or VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    // You should use methods like copyFromBufferToDeviceMemory instead.
    void 
    copyToHostMemory(void* sourceData, 
                     const VkDeviceSize size,
                     const VkDeviceSize offset);

    // This method will use as "size" the entire buffer size. 
    void 
    copyToHostMemory(void* sourceData,
                     const VkDeviceSize offset = 0);

    // These methods assumes the buffer was created with
    // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT.
    // The methods without size parameter are going to use
    // the sourceBuffer size.
    // The methods without the fence parameter, will create
    // an internal fence and wait for completion.
    //
    // * executionCompletedFence is a fence to be signaled once 
    //   the copy operation is complete. 
    void 
    copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                 const VkDeviceSize size,
                                 const CommandPool& transferCommandPool,
                                 const Fence& executionCompletedFence);
    void 
    copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                 const VkDeviceSize size,
                                 const CommandPool& transferCommandPool);
    void 
    copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                 const CommandPool& transferCommandPool,
                                 const Fence& executionCompletedFence);

    void 
    copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                 const CommandPool& transferCommandPool);

private:
    // Return the buffer memory requirements. This is used to create
    // the DeviceMemory (if needed).
    // Memory requirements:
    // - size is the size, in bytes, of the memory allocation required for the resource.
    // - alignment is the alignment, in bytes, of the offset within the 
    //   allocation required for the resource.
    // - memoryTypeBits is a bitmaskand contains one bit set for every supported 
    //   memory type for the resource.Bit i is set ifand only if the memory type i in 
    //   the VkPhysicalDeviceMemoryProperties structure for the physical device 
    //   is supported for the resource.
    VkMemoryRequirements 
    bufferMemoryRequirements() const;

    // Read Buffer() constructor to understand the parameters.
    static VkBuffer 
    createBuffer(const LogicalDevice& logicalDevice,
                 const VkDeviceSize sizeInBytes,
                 const VkBufferUsageFlags usageFlags,
                 const VkSharingMode sharingMode);

    const LogicalDevice& mLogicalDevice;    
    VkBuffer mBuffer = VK_NULL_HANDLE;
    const VkDeviceSize mSizeInBytes = 0;

    // This is only used if the Buffer is created
    // with the constructor that includes the data needed
    // for DeviceMemory creation.
    // Otherwise, we will use the DeviceMemory provided
    // by the second constructor.
    const bool mHasDeviceMemoryOwnership = true;
    const DeviceMemory* mDeviceMemory = nullptr;
};
}

#endif 