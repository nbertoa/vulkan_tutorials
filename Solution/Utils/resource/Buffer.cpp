#include "Buffer.h"

#include <cassert>

#include "../CommandPools.h"
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
Buffer::copyToHostMemory(const void* sourceData,                         
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
Buffer::copyToHostMemory(const void* sourceData,
                         const vk::DeviceSize offset) {
    copyToHostMemory(sourceData,                     
                     mSizeInBytes,
                     offset);
}

void
Buffer::copyFromBufferToDeviceMemory(const Buffer& sourceBuffer) {

    // Fence to be signaled once
    // the copy operation is complete. 
    vk::UniqueFence fence = LogicalDevice::device().createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
    LogicalDevice::device().waitForFences({fence.get()},
                                          VK_TRUE,
                                          std::numeric_limits<uint64_t>::max());
    LogicalDevice::device().resetFences({fence.get()});
    
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.setCommandBufferCount(1);
    allocInfo.setCommandPool(CommandPools::transferCommandPool());
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    vk::UniqueCommandBuffer commandBuffer = std::move(LogicalDevice::device().allocateCommandBuffersUnique(allocInfo).front());

    commandBuffer->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    vk::BufferCopy bufferCopy;
    bufferCopy.size = sourceBuffer.size();
    commandBuffer->copyBuffer(sourceBuffer.vkBuffer(),
                              mBuffer,
                              {bufferCopy});

    commandBuffer->end();
    
    vk::SubmitInfo info;
    info.setCommandBufferCount(1);
    info.setPCommandBuffers(&commandBuffer.get());
    const vk::PipelineStageFlags flags(vk::PipelineStageFlagBits::eTransfer);
    info.setPWaitDstStageMask(&flags);
    LogicalDevice::transferQueue().submit({info},
                                          fence.get());

    LogicalDevice::device().waitForFences({fence.get()},
                                          VK_TRUE,
                                          std::numeric_limits<uint64_t>::max());
}

void
Buffer::copyFromDataToDeviceMemory(const void* sourceData,
                                   const vk::DeviceSize size) {
    assert(sourceData != nullptr);
    assert(size > 0);

    Buffer buffer = createAndFillStagingBuffer(sourceData,
                                               size);

    copyFromBufferToDeviceMemory(buffer);
}

Buffer
Buffer::createAndFillStagingBuffer(const void* sourceData,
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