#include "CommandPool.h"

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
    if (type == Type::GRAPHICS) {
        createInfo.queueFamilyIndex = mLogicalDevice.physicalDevice().graphicsSupportQueueFamilyIndex();
    } else if (type == Type::TRANSFER) {
        createInfo.queueFamilyIndex = mLogicalDevice.physicalDevice().transferSupportQueueFamilyIndex();
    } else {
        assert(type == Type::PRESENTATION);
        createInfo.queueFamilyIndex = mLogicalDevice.physicalDevice().presentationSupportQueueFamilyIndex();
    }

    // There are two posible flags for command pools:
    // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers
    // are rerecorded with new commands very often (may change memory
    // allocation behavior)
    // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow 
    // command buffers to be rerecorded individually, without this flag
    // they all have to be reset together.
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
}