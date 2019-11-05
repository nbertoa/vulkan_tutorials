#include "CommandPool.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"

namespace vk {
CommandPool::CommandPool(const LogicalDevice& logicalDevice,
                         const uint32_t queueFamilyIndex,
                         const VkCommandPoolCreateFlags flags)
    : mLogicalDevice(logicalDevice)
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.queueFamilyIndex = queueFamilyIndex;
    
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