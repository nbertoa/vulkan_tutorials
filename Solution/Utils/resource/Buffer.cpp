#include "Buffer.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../command/CommandBuffer.h"
#include "../command/CommandPool.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"
#include "../sync/Fence.h"
#include "DeviceMemory.h"

namespace vk {
Buffer::Buffer(const LogicalDevice& logicalDevice,
               const PhysicalDevice& physicalDevice,
               const VkDeviceSize size,
               const VkBufferUsageFlags usageFlags,               
               const VkMemoryPropertyFlags memoryPropertyFlags,
               const VkSharingMode sharingMode,
               const std::vector<uint32_t>& queueFamilyIndices)
    : mLogicalDevice(logicalDevice)
    , mBuffer(createBuffer(logicalDevice,
                           size,
                           usageFlags,
                           sharingMode,
                           queueFamilyIndices))
    , mSizeInBytes(size)
    , mHasDeviceMemoryOwnership(true)
    , mDeviceMemory(new DeviceMemory(mLogicalDevice,
                                     physicalDevice,
                                     bufferMemoryRequirements(),
                                     memoryPropertyFlags))
{
    assert(mSizeInBytes > 0);

    // - device is the logical device that owns the buffer and memory.
    // - buffer to be attached to memory.
    // - memory is the device memory to attach.
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
               const DeviceMemory& deviceMemory,
               const VkSharingMode sharingMode,
               const std::vector<uint32_t>& queueFamilyIndices)
    : mLogicalDevice(logicalDevice)
    , mBuffer(createBuffer(logicalDevice,
                           size,
                           usageFlags,
                           sharingMode,
                           queueFamilyIndices))
    , mSizeInBytes(size)
    , mHasDeviceMemoryOwnership(false)
    , mDeviceMemory(&deviceMemory) {
    assert(mSizeInBytes > 0);

    // - device is the logical device that owns the buffer and memory.
    // - buffer to be attached to memory.
    // - memory is the device memory to attach.
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
    , mDeviceMemory(other.mDeviceMemory)
{
    other.mBuffer = VK_NULL_HANDLE;
    other.mDeviceMemory = nullptr;
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
    // - memory to be mapped.
    // - offset (zero-based byte) from the beginning of the memory object.
    // - size of the memory range to map, or VK_WHOLE_SIZE to map from 
    //   offset to the end of the allocation.
    // - ppData will contain a host-accessible pointer to the beginning of the mapped range.
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
    // - memory to be unmapped.
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
                     const VkSharingMode sharingMode,
                     const std::vector<uint32_t>& queueFamilyIndices) {
    VkBuffer buffer;

    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usageFlags;
    createInfo.sharingMode = sharingMode;
    createInfo.flags = 0;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
    createInfo.pQueueFamilyIndices = queueFamilyIndices.empty() ? nullptr : queueFamilyIndices.data();
    
    vkChecker(vkCreateBuffer(logicalDevice.vkDevice(),
                             &createInfo,
                             nullptr,
                             &buffer));

    assert(buffer != VK_NULL_HANDLE);

    return buffer;
}

}