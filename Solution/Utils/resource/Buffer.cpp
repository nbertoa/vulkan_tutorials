#include "Buffer.h"

#include <cassert>

#include "DeviceMemory.h"
#include "../DebugUtils.h"
#include "../command/CommandBuffer.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"

namespace vk2 {
Buffer::Buffer(const VkDeviceSize bufferSize,
               const VkBufferUsageFlags bufferUsage,
               const VkMemoryPropertyFlags deviceMemoryProperties,
               const VkSharingMode sharingMode,
               const std::vector<uint32_t>& queueFamilyIndices)
    : mBuffer(createBuffer(bufferSize,
                           bufferUsage,
                           sharingMode,
                           queueFamilyIndices))
    , mSizeInBytes(bufferSize)
    , mHasDeviceMemoryOwnership(true)
    , mDeviceMemory(new DeviceMemory(bufferMemoryRequirements(),
                                     deviceMemoryProperties))
{
    assert(mSizeInBytes > 0);

    // Binds device memory to this buffer
    vkChecker(vkBindBufferMemory(LogicalDevice::device(),
                                 mBuffer,
                                 mDeviceMemory->vkDeviceMemory(),
                                 0));
}

Buffer::Buffer(const VkDeviceSize bufferSize,
               const VkBufferUsageFlags bufferUsage,
               const DeviceMemory& deviceMemory,
               const VkSharingMode sharingMode,
               const std::vector<uint32_t>& queueFamilyIndices)
    : mBuffer(createBuffer(bufferSize,
                           bufferUsage,
                           sharingMode,
                           queueFamilyIndices))
    , mSizeInBytes(bufferSize)
    , mHasDeviceMemoryOwnership(false)
    , mDeviceMemory(&deviceMemory) {
    assert(mSizeInBytes > 0);

    // Binds device memory to this buffer
    vkChecker(vkBindBufferMemory(LogicalDevice::device(),
                                 mBuffer,
                                 mDeviceMemory->vkDeviceMemory(),
                                 0));
}

Buffer::~Buffer() {
    vkDestroyBuffer(LogicalDevice::device(),
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
    : mBuffer(other.mBuffer)
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
    //   This pointer minus offset must be aligned to at least 
    //   VkPhysicalDeviceLimits::minMemoryMapAlignment.
    vkChecker(vkMapMemory(LogicalDevice::device(),
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
    vkUnmapMemory(LogicalDevice::device(),
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
                                     const CommandPool& transferCommandPool) {

    // Fence to be signaled once
    // the copy operation is complete. 
    vk::Device device(LogicalDevice::device());
    vk::UniqueFence fence = device.createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
    vkChecker(device.waitForFences({fence.get()},
                                   VK_TRUE,
                                   std::numeric_limits<uint64_t>::max()));
    device.resetFences({fence.get()});

    CommandBuffer commandBuffer(transferCommandPool,
                                VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    commandBuffer.beginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy bufferCopy = {};
    bufferCopy.size = sourceBuffer.size();
    commandBuffer.copyBuffer(sourceBuffer,
                             *this,
                             bufferCopy);

    commandBuffer.endRecording();

    commandBuffer.submit(LogicalDevice::transferQueue(),
                         nullptr,
                         nullptr,
                         fence.get(),
                         VK_PIPELINE_STAGE_TRANSFER_BIT);

    vkChecker(device.waitForFences({fence.get()},
                                   VK_TRUE,
                                   std::numeric_limits<uint64_t>::max()));
}

void
Buffer::copyFromDataToDeviceMemory(void* sourceData,
                                   const VkDeviceSize size,
                                   const CommandPool& transferCommandPool) {
    assert(sourceData != nullptr);
    assert(size > 0);

    Buffer buffer = createAndFillStagingBuffer(sourceData,
                                               size);

    copyFromBufferToDeviceMemory(buffer,
                                 transferCommandPool);
}

Buffer
Buffer::createAndFillStagingBuffer(void* sourceData,
                                   const VkDeviceSize size) {
    assert(sourceData != nullptr);
    assert(size > 0);

    Buffer buffer(size,
                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    buffer.copyToHostMemory(sourceData,
                            size,
                            0);

    return buffer;
}

VkMemoryRequirements 
Buffer::bufferMemoryRequirements() const {
    assert(mBuffer != VK_NULL_HANDLE);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(LogicalDevice::device(),
                                  mBuffer,
                                  &memoryRequirements);

    return memoryRequirements;
}

VkBuffer
Buffer::createBuffer(const VkDeviceSize size,
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
    createInfo.pQueueFamilyIndices = queueFamilyIndices.empty() ? 
        nullptr : 
        queueFamilyIndices.data();
    
    vkChecker(vkCreateBuffer(LogicalDevice::device(),
                             &createInfo,
                             nullptr,
                             &buffer));

    assert(buffer != VK_NULL_HANDLE);

    return buffer;
}

}