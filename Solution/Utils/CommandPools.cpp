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
}