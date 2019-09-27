#include "CommandPool.h"

#include <cassert>

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
CommandPool::CommandPool(const LogicalDevice& logicalDevice,
                         const Type type,
                         const VkCommandPoolCreateFlags flags)
    : mLogicalDevice(logicalDevice)
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // Command buffers are executed by submitting them on one of the device queues,
    // like the graphics and presentation queues. Each command pool can only allocate
    // command buffers that are submitted on a single type of queue.
    const PhysicalDevice& physicalDevice = mLogicalDevice.physicalDevice();
    if (type == Type::GRAPHICS) {
        createInfo.queueFamilyIndex = physicalDevice.graphicsSupportQueueFamilyIndex();
    } else if (type == Type::TRANSFER) {
        createInfo.queueFamilyIndex = physicalDevice.transferSupportQueueFamilyIndex();
    } else {
        assert(type == Type::PRESENTATION);
        createInfo.queueFamilyIndex = physicalDevice.presentationSupportQueueFamilyIndex();
    }

    createInfo.flags = flags;
    
    vkChecker(vkCreateCommandPool(mLogicalDevice.vkDevice(),
                                  &createInfo,
                                  nullptr,
                                  &mCommandPool));
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(mLogicalDevice.vkDevice(),
                         mCommandPool,
                         nullptr);
}

CommandPool::CommandPool(CommandPool&& other) noexcept 
    : mLogicalDevice(other.mLogicalDevice)
    , mCommandPool(other.mCommandPool)
{
    other.mCommandPool = VK_NULL_HANDLE;
}

VkCommandPool 
CommandPool::vkCommandPool() const {
    assert(mCommandPool != VK_NULL_HANDLE);
    return mCommandPool;
}
}