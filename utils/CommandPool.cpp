#include "CommandPool.h"

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
CommandPool::CommandPool(const LogicalDevice& logicalDevice,
                         const Type type)
    : mLogicalDevice(logicalDevice)
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = 
        type == Type::GRAPHICS ? mLogicalDevice.physicalDevice().graphicsSupportQueueFamilyIndex()
                               : mLogicalDevice.physicalDevice().presentationSupportQueueFamilyIndex();
    
    vkChecker(vkCreateCommandPool(mLogicalDevice.vkDevice(),
                                  &createInfo,
                                  nullptr,
                                  &mCommandPool));
}

CommandPool::~CommandPool() {
    assert(mCommandPool != VK_NULL_HANDLE);

    vkDestroyCommandPool(mLogicalDevice.vkDevice(),
                         mCommandPool,
                         nullptr);
}
}