#include "CommandPools.h"

#include "device/LogicalDevice.h"
#include "device/PhysicalDevice.h"

namespace vulkan {
vk::UniqueCommandPool 
CommandPools::mGraphicsCommandPool = vk::UniqueCommandPool();

vk::UniqueCommandPool 
CommandPools::mTransferCommandPool = vk::UniqueCommandPool();

void
CommandPools::initialize() {
    assert(mGraphicsCommandPool.get() == VK_NULL_HANDLE);
    assert(mTransferCommandPool.get() == VK_NULL_HANDLE);

    vk::CommandPoolCreateInfo info;
    info.setQueueFamilyIndex(PhysicalDevice::graphicsQueueFamilyIndex());
    mGraphicsCommandPool = LogicalDevice::device().createCommandPoolUnique(info);

    info.setQueueFamilyIndex(PhysicalDevice::transferQueueFamilyIndex());
    info.setFlags(vk::CommandPoolCreateFlagBits::eTransient);
    mTransferCommandPool = LogicalDevice::device().createCommandPoolUnique(info);
}

void
CommandPools::finalize() {
    mGraphicsCommandPool.reset();
    mTransferCommandPool.reset();
}

vk::CommandPool
CommandPools::graphicsCommandPool() {
    assert(mGraphicsCommandPool.get() != VK_NULL_HANDLE);
    return mGraphicsCommandPool.get();
}

vk::CommandPool
CommandPools::transferCommandPool() {
    assert(mTransferCommandPool.get() != VK_NULL_HANDLE);
    return mTransferCommandPool.get();
}

vk::UniqueCommandBuffer
CommandPools::beginOneTimeSubmitCommandBuffer() {
    assert(mTransferCommandPool.get() != VK_NULL_HANDLE);

    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.setCommandBufferCount(1);
    allocInfo.setCommandPool(mTransferCommandPool.get());
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);

    vk::UniqueCommandBuffer commandBuffer = std::move(LogicalDevice::device().allocateCommandBuffersUnique(allocInfo).front());
    commandBuffer->begin(vk::CommandBufferBeginInfo {vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    return commandBuffer;
}

void
CommandPools::endAndWaitOneTimeSubmitCommandBuffer(vk::CommandBuffer commandBuffer) {
    assert(commandBuffer != VK_NULL_HANDLE);

    commandBuffer.end();

    vk::UniqueFence fence = LogicalDevice::device().createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
    LogicalDevice::device().waitForFences({fence.get()},
                                          VK_TRUE,
                                          std::numeric_limits<uint64_t>::max());
    LogicalDevice::device().resetFences({fence.get()});

    vk::SubmitInfo info;
    info.setCommandBufferCount(1);
    info.setPCommandBuffers(&commandBuffer);
    const vk::PipelineStageFlags flags(vk::PipelineStageFlagBits::eTransfer);
    info.setPWaitDstStageMask(&flags);
    LogicalDevice::transferQueue().submit({info},
                                          fence.get());

    LogicalDevice::device().waitForFences({fence.get()},
                                          VK_TRUE,
                                          std::numeric_limits<uint64_t>::max());
}
}