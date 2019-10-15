#ifndef UTILS_BUFFERS
#define UTILS_BUFFERS

#include <vector>
#include <vulkan/vulkan.h>

#include "Buffer.h"

namespace vk {
class LogicalDevice;
class PhysicalDevice;

class Buffers {
public:
    // Buffer Usage Flags:
    // - VK_BUFFER_USAGE_TRANSFER_SRC_BIT specifies that the buffer can be used 
    //   as the source of a transfer command(see the definition of VK_PIPELINE_STAGE_TRANSFER_BIT).
    //
    // - VK_BUFFER_USAGE_TRANSFER_DST_BIT specifies that the buffer can be used as 
    //   the destination of a transfer command.
    //
    // - VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT specifies that the buffer can be used 
    //   to create a VkBufferView suitable for occupying a VkDescriptorSet slot of type 
    //   VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER.
    //
    // - VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT specifies that the buffer can be used 
    //   to create a VkBufferView suitable for occupying a VkDescriptorSet slot of type 
    //   VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER.
    //
    // - VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT specifies that the buffer can be used in a 
    //   VkDescriptorBufferInfo suitable for occupying a VkDescriptorSet slot either of 
    //   type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC.
    //
    // - VK_BUFFER_USAGE_STORAGE_BUFFER_BIT specifies that the buffer can be used in a 
    //   VkDescriptorBufferInfo suitable for occupying a VkDescriptorSet slot either of type 
    //   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER or VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC.
    //
    // - VK_BUFFER_USAGE_INDEX_BUFFER_BIT specifies that the buffer is suitable for passing 
    //   as the buffer parameter to vkCmdBindIndexBuffer.
    //
    // - VK_BUFFER_USAGE_VERTEX_BUFFER_BIT specifies that the buffer is suitable for passing 
    //   as an element of the pBuffers array to vkCmdBindVertexBuffers.
    //
    // - VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT specifies that the buffer is suitable for passing 
    //   as the buffer parameter to vkCmdDrawIndirect, vkCmdDrawIndexedIndirect, 
    //   vkCmdDrawMeshTasksIndirectNV, vkCmdDrawMeshTasksIndirectCountNV, or vkCmdDispatchIndirect.
    //   It is also suitable for passing as the buffer member of VkIndirectCommandsTokenNVX, or 
    //   sequencesCountBuffer or sequencesIndexBuffer member of VkCmdProcessCommandsInfoNVX
    //
    // - VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT specifies that the buffer is suitable for 
    //   passing as the buffer parameter to vkCmdBeginConditionalRenderingEXT.
    //
    // - VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT specifies that the buffer is suitable 
    //   for using for binding as a transform feedback buffer with vkCmdBindTransformFeedbackBuffersEXT.
    //
    // - VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT specifies that the buffer is 
    //   suitable for using as a counter buffer with vkCmdBeginTransformFeedbackEXTand vkCmdEndTransformFeedbackEXT.
    //
    // - VK_BUFFER_USAGE_RAY_TRACING_BIT_NV specifies that the buffer is suitable for use 
    //   in vkCmdTraceRaysNVand vkCmdBuildAccelerationStructureNV.
    //
    // - VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT specifies that the buffer can be 
    //   used to retrieve a buffer device address via vkGetBufferDeviceAddressEXTand use 
    //   that address to access the buffer’s memory from a shader.
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
    // - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit specifies that memory allocated with 
    //   this type is the most efficient for device access.
    //   This property will be set ifand only if the memory type belongs to a heap 
    //   with the VK_MEMORY_HEAP_DEVICE_LOCAL_BIT set.
    //
    // - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with 
    //   this type can be mapped for host access using vkMapMemory.
    //
    // - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache 
    //   management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
    //   are not needed to flush host writes to the device or make device writes visible 
    //   to the host, respectively.
    //
    // - VK_MEMORY_PROPERTY_HOST_CACHED_BIT bit specifies that memory allocated with this 
    //   type is cached on the host.Host memory accesses to uncached memory are slower than 
    //   to cached memory, however uncached memory is always host coherent.
    //
    // - VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit specifies that the memory type only 
    //   allows device access to the memory.Memory types must not have both 
    //   K_MEMORY_PROPERTY_LAZILY_ALLOCATED_BITand VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set.
    //   Additionally, the object’s backing memory may be provided by the implementation lazily 
    //   as specified in Lazily Allocated Memory.
    //
    // - VK_MEMORY_PROPERTY_PROTECTED_BIT bit specifies that the memory type only allows device 
    //   access to the memory, and allows protected queue operations to access the memory.
    //   Memory types must not have VK_MEMORY_PROPERTY_PROTECTED_BIT setand any of 
    //   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set, or VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, 
    //   or VK_MEMORY_PROPERTY_HOST_CACHED_BIT set.
    //
    // - VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD bit specifies that device accesses to 
    //   allocations of this memory type are automatically made availableand visible.
    //
    // - VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD bit specifies that memory allocated with 
    //   this type is not cached on the device.Uncached device memory is always device coherent.
    Buffers(const LogicalDevice& logicalDevice,
            const PhysicalDevice& physicalDevice,
            const size_t bufferCount,
            const VkDeviceSize sizeInBytes,
            const VkBufferUsageFlags usageFlags,
            const VkSharingMode sharingMode,
            const VkMemoryPropertyFlags memoryPropertyFlags);
    Buffers(Buffers&& other) noexcept;
    Buffers(const Buffers&) = delete;
    const Buffers& operator=(const Buffers&) = delete;

    size_t bufferCount() const;

    Buffer& buffer(const size_t bufferIndex);

private:
    std::vector<Buffer> mBuffers;
};
}

#endif