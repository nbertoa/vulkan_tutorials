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
    // Command buffers are executed by submitting them on one of the device queues,
    // like the graphics and presentation queues. Each command pool can only allocate
    // command buffers that are submitted on a single type of queue.
    // We are going to record commands for drawing, which is wyh we have chosen
    // the graphics queue family.
    createInfo.queueFamilyIndex = 
        type == Type::GRAPHICS ? mLogicalDevice.physicalDevice().graphicsSupportQueueFamilyIndex()
                               : mLogicalDevice.physicalDevice().presentationSupportQueueFamilyIndex();

    // There are two posible flags for command pools:
    // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers
    // are rerecorded with new commands very often (may change memory
    // allocation behavior)
    // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow 
    // command buffers to be rerecorded individually, without this flag
    // they all have to be reset together.
    // 
    // We will only record the command buffers at the beginning
    // of the program and then execute them many times in the main loop,
    // so we are not going to use either of these flags.
    createInfo.flags = 0;
    
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
}