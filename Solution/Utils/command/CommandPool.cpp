#include "CommandPool.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"

namespace vk2 {
CommandPool::CommandPool(const uint32_t queueFamilyIndex,
                         const VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.queueFamilyIndex = queueFamilyIndex;
    
    vkChecker(vkCreateCommandPool(LogicalDevice::vkDevice(),
                                  &createInfo,
                                  nullptr,
                                  &mCommandPool));
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(LogicalDevice::vkDevice(),
                         mCommandPool,
                         nullptr);
}

CommandPool::CommandPool(CommandPool&& other) noexcept 
    : mCommandPool(other.mCommandPool)
{
    other.mCommandPool = VK_NULL_HANDLE;
}

VkCommandPool 
CommandPool::vkCommandPool() const {
    assert(mCommandPool != VK_NULL_HANDLE);
    return mCommandPool;
}

CommandBuffer
CommandPool::createAndBeginOneTimeSubmitCommandBuffer() const {
    assert(mCommandPool != VK_NULL_HANDLE);

    CommandBuffer commandBuffer(*this,
                                VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    commandBuffer.beginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    return commandBuffer;
}
}