#include "CommandBuffers.h"

#include <cassert>

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DebugUtils.h"
#include "GraphicsPipeline.h"
#include "LogicalDevice.h"
#include "RenderPass.h"

namespace vk {
CommandBuffers::CommandBuffers(const LogicalDevice& logicalDevice,
                               const CommandPool& commandPool,
                               const size_t bufferCount,
                               const VkCommandBufferLevel level)
{
    assert(bufferCount > 0);
    mCommandBuffers.reserve(bufferCount);
    std::vector<VkCommandBuffer> commandBuffers(bufferCount);

    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = commandPool.vkCommandPool();
    info.level = level;
    info.commandBufferCount = static_cast<uint32_t>(bufferCount);

    vkChecker(vkAllocateCommandBuffers(logicalDevice.vkDevice(),
                                       &info,
                                       commandBuffers.data()));

    for (const VkCommandBuffer commandBuffer : commandBuffers) {
       assert(commandBuffer != VK_NULL_HANDLE);
       mCommandBuffers.push_back(CommandBuffer(commandBuffer));
    }
}

CommandBuffers::CommandBuffers(CommandBuffers&& other) noexcept 
    : mCommandBuffers(std::move(other.mCommandBuffers))
{

}

size_t 
CommandBuffers::bufferCount() const {
    assert(mCommandBuffers.empty() == false);
    return mCommandBuffers.size();
}

CommandBuffer& 
CommandBuffers::commandBuffer(const size_t bufferIndex) {
    assert(bufferIndex < mCommandBuffers.size());
    return mCommandBuffers[bufferIndex];
}
}