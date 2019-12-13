#include "Buffer.h"

#include <cassert>

#include "../command/CommandBuffer.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"

namespace vulkan {
Buffer::Buffer(const vk::DeviceSize bufferSize,
               const vk::BufferUsageFlags bufferUsage,
               const vk::MemoryPropertyFlags deviceMemoryProperties,
               const vk::SharingMode sharingMode,
               const std::vector<uint32_t>& queueFamilyIndices)
    : mBuffer(createBuffer(bufferSize,
                           bufferUsage,
                           sharingMode,
                           queueFamilyIndices))
    , mSizeInBytes(bufferSize)
    , mHasDeviceMemoryOwnership(true)
{
    const vk::MemoryRequirements memoryRequirements = LogicalDevice::device().getBufferMemoryRequirements(mBuffer);
    vk::MemoryAllocateInfo info;
    info.setAllocationSize(memoryRequirements.size);
    info.setMemoryTypeIndex(PhysicalDevice::memoryTypeIndex(memoryRequirements.memoryTypeBits,
                                                            deviceMemoryProperties));

    mDeviceMemory = LogicalDevice::device().allocateMemory(info);

    assert(mSizeInBytes > 0);

    LogicalDevice::device().bindBufferMemory(mBuffer,
                                             mDeviceMemory,
                                             0); // offset
}

Buffer::Buffer(const vk::DeviceSize bufferSize,
               const vk::BufferUsageFlags bufferUsage,
               const vk::DeviceMemory deviceMemory,
               const vk::SharingMode sharingMode,
               const std::vector<uint32_t>& queueFamilyIndices)
    : mBuffer(createBuffer(bufferSize,
                           bufferUsage,
                           sharingMode,
                           queueFamilyIndices))
    , mSizeInBytes(bufferSize)
    , mHasDeviceMemoryOwnership(false)
    , mDeviceMemory(deviceMemory) {
    assert(mSizeInBytes > 0);

    LogicalDevice::device().bindBufferMemory(mBuffer,
                                             mDeviceMemory,
                                             0); // offset
}

Buffer::~Buffer() {
    vkDestroyBuffer(LogicalDevice::device(),
                    mBuffer,
                    nullptr);

    if (mHasDeviceMemoryOwnership) {
        LogicalDevice::device().freeMemory(mDeviceMemory);
    }
}

vk::Buffer 
Buffer::vkBuffer() const {
    assert(mBuffer != VK_NULL_HANDLE);
    return mBuffer;
}

vk::DeviceSize 
Buffer::size() const {
    assert(mBuffer != VK_NULL_HANDLE);
    return mSizeInBytes;
}

Buffer::Buffer(Buffer&& other) noexcept 
    : mBuffer(other.mBuffer)
    , mSizeInBytes(other.mSizeInBytes)
    , mDeviceMemory(other.mDeviceMemory)
{
    other.mBuffer = vk::Buffer();
    other.mDeviceMemory = nullptr;
}

void 
Buffer::copyToHostMemory(void* sourceData,                         
                         const vk::DeviceSize size,
                         const vk::DeviceSize offset) {
    assert(mBuffer != VK_NULL_HANDLE);
    assert(sourceData != nullptr);
    assert(size > 0);

    void* destinationData = LogicalDevice::device().mapMemory(mDeviceMemory,
                                                              offset,
                                                              size);

    memcpy(destinationData,
           sourceData,
           static_cast<size_t>(size));

    LogicalDevice::device().unmapMemory(mDeviceMemory);
}

void
Buffer::copyToHostMemory(void* sourceData,
                         const vk::DeviceSize offset) {
    copyToHostMemory(sourceData,                     
                     mSizeInBytes,
                     offset);
}

void
Buffer::copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                     const vk::CommandPool transferCommandPool) {

    // Fence to be signaled once
    // the copy operation is complete. 
    vk::UniqueFence fence = LogicalDevice::device().createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
    LogicalDevice::device().waitForFences({fence.get()},
                                          VK_TRUE,
                                          std::numeric_limits<uint64_t>::max());
    LogicalDevice::device().resetFences({fence.get()});
    
    CommandBuffer commandBuffer(transferCommandPool,
                                vk::CommandBufferLevel::ePrimary);
    commandBuffer.beginRecording(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    vk::BufferCopy bufferCopy;
    bufferCopy.size = sourceBuffer.size();
    commandBuffer.copyBuffer(sourceBuffer,
                             *this,
                             bufferCopy);

    commandBuffer.endRecording();

    commandBuffer.submit(LogicalDevice::transferQueue(),
                         nullptr,
                         nullptr,
                         fence.get(),
                         vk::PipelineStageFlagBits::eTransfer);

    LogicalDevice::device().waitForFences({fence.get()},
                                          VK_TRUE,
                                          std::numeric_limits<uint64_t>::max());
}

void
Buffer::copyFromDataToDeviceMemory(void* sourceData,
                                   const vk::DeviceSize size,
                                   const vk::CommandPool transferCommandPool) {
    assert(sourceData != nullptr);
    assert(size > 0);

    Buffer buffer = createAndFillStagingBuffer(sourceData,
                                               size);

    copyFromBufferToDeviceMemory(buffer,
                                 transferCommandPool);
}

Buffer
Buffer::createAndFillStagingBuffer(void* sourceData,
                                   const vk::DeviceSize size) {
    assert(sourceData != nullptr);
    assert(size > 0);

    Buffer buffer(size,
                  vk::BufferUsageFlagBits::eTransferSrc,
                  vk::MemoryPropertyFlagBits::eHostVisible |
                  vk::MemoryPropertyFlagBits::eHostCoherent);

    buffer.copyToHostMemory(sourceData,
                            size,
                            0);

    return buffer;
}

vk::Buffer
Buffer::createBuffer(const vk::DeviceSize size,
                     const vk::BufferUsageFlags usageFlags,
                     const vk::SharingMode sharingMode,
                     const std::vector<uint32_t>& queueFamilyIndices) {

    vk::BufferCreateInfo info;
    info.setSize(size);
    info.setUsage(usageFlags);
    info.setSharingMode(sharingMode);
    info.setQueueFamilyIndexCount(static_cast<uint32_t>(queueFamilyIndices.size()));
    info.setPQueueFamilyIndices(queueFamilyIndices.empty() ?
                                nullptr :
                                queueFamilyIndices.data());

    return LogicalDevice::device().createBuffer(info);
}

}