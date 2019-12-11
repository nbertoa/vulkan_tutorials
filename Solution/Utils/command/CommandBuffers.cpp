#include "CommandBuffers.h"

#include <cassert>

#include "CommandBuffer.h"
#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"
#include "../pipeline/GraphicsPipeline.h"

namespace vk2 {
CommandBuffers::CommandBuffers(const vk::CommandPool commandPool,
                               const size_t bufferCount,
                               const VkCommandBufferLevel level)
{
    assert(bufferCount > 0);
    mCommandBuffers.reserve(bufferCount);
    std::vector<VkCommandBuffer> commandBuffers(bufferCount);

    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = commandPool;
    info.level = level;
    info.commandBufferCount = static_cast<uint32_t>(bufferCount);

    vkChecker(vkAllocateCommandBuffers(LogicalDevice::device(),
                                       &info,
                                       commandBuffers.data()));

    for (const VkCommandBuffer commandBuffer : commandBuffers) {
       assert(commandBuffer != VK_NULL_HANDLE);
       mCommandBuffers.emplace_back(std::move(CommandBuffer(commandBuffer)));
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