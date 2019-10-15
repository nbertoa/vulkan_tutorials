#include "Buffer.h"

#include <cassert>

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DebugUtils.h"
#include "Fence.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"

namespace vk {
Buffer::Buffer(const LogicalDevice& logicalDevice,
               const PhysicalDevice& physicalDevice,
               const VkDeviceSize size,
               const VkBufferUsageFlags usageFlags,
               const VkSharingMode sharingMode,
               const VkMemoryPropertyFlags memoryPropertyFlags)
    : mLogicalDevice(logicalDevice)
    , mBuffer(createBuffer(logicalDevice,
                           size,
                           usageFlags,
                           sharingMode))
    , mSizeInBytes(size)
    , mHasDeviceMemoryOwnership(true)
    , mDeviceMemory(new DeviceMemory(mLogicalDevice,
                                     physicalDevice,
                                     bufferMemoryRequirements(),
                                     memoryPropertyFlags))
{
    assert(mSizeInBytes > 0);

    // - device is the logical device that owns the bufferand memory.
    // - buffer is the buffer to be attached to memory.
    // - memory is a VkDeviceMemory object describing the device memory to attach.
    // - memoryOffset is the start offset of the region of memory which is to be bound to the buffer.
    //   The number of bytes returned in the VkMemoryRequirements::size member in memory, 
    //   starting from memoryOffset bytes, will be bound to the specified buffer.
    //   If the offset is non-zero, then it is required to be divisible by memory requirements
    //   aligment field.
    vkChecker(vkBindBufferMemory(mLogicalDevice.vkDevice(),
                                 mBuffer,
                                 mDeviceMemory->vkDeviceMemory(),
                                 0));
}

Buffer::Buffer(const LogicalDevice& logicalDevice,
               const VkDeviceSize size,
               const VkBufferUsageFlags usageFlags,
               const VkSharingMode sharingMode,
               const DeviceMemory& deviceMemory)
    : mLogicalDevice(logicalDevice)
    , mBuffer(createBuffer(logicalDevice,
                           size,
                           usageFlags,
                           sharingMode))
    , mSizeInBytes(size)
    , mHasDeviceMemoryOwnership(false)
    , mDeviceMemory(&deviceMemory) {
    assert(mSizeInBytes > 0);

    // - device is the logical device that owns the bufferand memory.
    // - buffer is the buffer to be attached to memory.
    // - memory is a VkDeviceMemory object describing the device memory to attach.
    // - memoryOffset is the start offset of the region of memory which is to be bound to the buffer.
    //   The number of bytes returned in the VkMemoryRequirements::size member in memory, 
    //   starting from memoryOffset bytes, will be bound to the specified buffer.
    //   If the offset is non-zero, then it is required to be divisible by memory requirements
    //   aligment field.
    vkChecker(vkBindBufferMemory(mLogicalDevice.vkDevice(),
                                 mBuffer,
                                 mDeviceMemory->vkDeviceMemory(),
                                 0));
}

Buffer::~Buffer() {
    vkDestroyBuffer(mLogicalDevice.vkDevice(),
                    mBuffer,
                    nullptr);

    if (mHasDeviceMemoryOwnership) {
        delete mDeviceMemory;
    }
}

VkBuffer 
Buffer::vkBuffer() const {
    assert(mBuffer != VK_NULL_HANDLE);
    return mBuffer;
}

VkDeviceSize 
Buffer::size() const {
    assert(mBuffer != VK_NULL_HANDLE);
    return mSizeInBytes;
}

Buffer::Buffer(Buffer&& other) noexcept 
    : mLogicalDevice(other.mLogicalDevice)
    , mBuffer(other.mBuffer)
    , mSizeInBytes(other.mSizeInBytes)
    , mDeviceMemory(std::move(other.mDeviceMemory))
{
    other.mBuffer = VK_NULL_HANDLE;
}

void 
Buffer::copyToHostMemory(void* sourceData,                         
                         const VkDeviceSize size,
                         const VkDeviceSize offset) {
    assert(mBuffer != VK_NULL_HANDLE);
    assert(sourceData != nullptr);
    assert(size > 0);

    void* destinationData;
    // - device is the logical device that owns the memory.
    // - memory is the VkDeviceMemory object to be mapped.
    // - offset is a zero-based byte offset from the beginning of the memory object.
    // - size is the size of the memory range to map, or VK_WHOLE_SIZE to map from 
    //   offset to the end of the allocation.
    // - flags is reserved for future use.
    // - ppData is a pointer to a void* variable in which is returned a host-accessible pointer 
    //   to the beginning of the mapped range.
    //   This pointer minus offset must be aligned to at least VkPhysicalDeviceLimits::minMemoryMapAlignment.
    vkChecker(vkMapMemory(mLogicalDevice.vkDevice(),
                          mDeviceMemory->vkDeviceMemory(),
                          offset,
                          size,
                          0,
                          &destinationData));

    memcpy(destinationData,
           sourceData,
           static_cast<size_t>(size));

    // - device is the logical device that owns the memory.
    // - memory is the memory object to be unmapped.
    vkUnmapMemory(mLogicalDevice.vkDevice(),
                  mDeviceMemory->vkDeviceMemory());
}

void
Buffer::copyToHostMemory(void* sourceData,
                         const VkDeviceSize offset) {
    copyToHostMemory(sourceData,                     
                     mSizeInBytes,
                     offset);
}

void 
Buffer::copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                     const VkDeviceSize size,
                                     const CommandPool& transferCommandPool,
                                     const Fence& executionCompletedFence) {
    assert(size > 0);

    CommandBuffer commandBuffer(mLogicalDevice,
                                transferCommandPool,
                                VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    commandBuffer.beginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy bufferCopy = {};
    bufferCopy.size = size;
    commandBuffer.copyBuffer(sourceBuffer,
                             *this,
                             bufferCopy);

    commandBuffer.endRecording();

    commandBuffer.submit(mLogicalDevice.transferQueue(),
                         nullptr,
                         nullptr,
                         executionCompletedFence,
                         VK_PIPELINE_STAGE_TRANSFER_BIT);
}

void
Buffer::copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                     const VkDeviceSize size,
                                     const CommandPool& transferCommandPool) {
    Fence executionCompletedFence(mLogicalDevice);
    executionCompletedFence.waitAndReset();
    copyFromBufferToDeviceMemory(sourceBuffer,
                                 size,
                                 transferCommandPool,
                                 executionCompletedFence);
    executionCompletedFence.wait();
}

void
Buffer::copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                     const CommandPool& transferCommandPool,
                                     const Fence& executionCompletedFence) {
    copyFromBufferToDeviceMemory(sourceBuffer,
                                 sourceBuffer.size(),
                                 transferCommandPool,
                                 executionCompletedFence);
}

void
Buffer::copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                     const CommandPool& transferCommandPool) {
    Fence executionCompletedFence(mLogicalDevice);
    executionCompletedFence.waitAndReset();
    copyFromBufferToDeviceMemory(sourceBuffer,
                                 transferCommandPool,
                                 executionCompletedFence);
    executionCompletedFence.wait();
}

VkMemoryRequirements 
Buffer::bufferMemoryRequirements() const {
    assert(mBuffer != VK_NULL_HANDLE);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(mLogicalDevice.vkDevice(),
                                  mBuffer,
                                  &memoryRequirements);

    return memoryRequirements;
}

VkBuffer
Buffer::createBuffer(const LogicalDevice& logicalDevice,
                     const VkDeviceSize size,
                     const VkBufferUsageFlags usageFlags,
                     const VkSharingMode sharingMode) {
    VkBuffer buffer;

    // VkBufferCreateInfo:
    // - flags is a bitmask of VkBufferCreateFlagBits specifying additional parameters of the buffer.
    //   - VK_BUFFER_CREATE_SPARSE_BINDING_BIT specifies that the buffer will be backed using sparse memory binding.
    //   - VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT specifies that the buffer can be partially backed using sparse memory binding.
    //     Buffers created with this flag must also be created with the VK_BUFFER_CREATE_SPARSE_BINDING_BIT flag.
    //   - VK_BUFFER_CREATE_SPARSE_ALIASED_BIT specifies that the buffer will be backed using sparse memory binding 
    //     with memory ranges that might also simultaneously be backing another buffer(or another portion of the same buffer).
    //     Buffers created with this flag must also be created with the VK_BUFFER_CREATE_SPARSE_BINDING_BIT flag.
    //   - VK_BUFFER_CREATE_PROTECTED_BIT specifies that the buffer is a protected buffer.
    //   - VK_BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT_EXT specifies that the buffer’s address can be savedand 
    //     reused on a subsequent run(e.g. for trace capture and replay).
    // - size is the size in bytes of the buffer to be created.
    // - usage is a bitmask of VkBufferUsageFlagBits specifying allowed usages of the buffer.    
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
    // - sharingMode is a VkSharingMode value specifying the sharing mode of the buffer 
    //   when it will be accessed by multiple queue families.
    //   - VK_SHARING_MODE_EXCLUSIVE specifies that access 
    //     to any range or image subresource of the object 
    //     will be exclusive to a single queue family at a time.
    //   - VK_SHARING_MODE_CONCURRENT specifies that concurrent 
    //     access to any range or image subresource of the object 
    //     from multiple queue families is supported.
    // - queueFamilyIndexCount is the number of entries in the pQueueFamilyIndices array.
    // - pQueueFamilyIndices is a list of queue families that will access this buffer
    //   (ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT).
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usageFlags;
    createInfo.sharingMode = sharingMode;
    createInfo.flags = 0;

    vkChecker(vkCreateBuffer(logicalDevice.vkDevice(),
                             &createInfo,
                             nullptr,
                             &buffer));

    assert(buffer != VK_NULL_HANDLE);

    return buffer;
}

}