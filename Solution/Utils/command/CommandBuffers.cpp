#include "CommandBuffers.h"

#include <cassert>

#include "CommandBuffer.h"
#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"
#include "../pipeline/GraphicsPipeline.h"

namespace vk2 {
CommandBuffers::CommandBuffers(const vk::CommandPool commandPool,
                               const size_t bufferCount,
                               const vk::CommandBufferLevel level)
{
    assert(bufferCount > 0);

    vk::CommandBufferAllocateInfo info;
    info.setCommandBufferCount(static_cast<uint32_t>(bufferCount));
    info.setLevel(level);
    info.setCommandPool(commandPool);
    
    std::vector<vk::CommandBuffer> commandBuffers = LogicalDevice::device().allocateCommandBuffers(info);
    
    for (const vk::CommandBuffer commandBuffer : commandBuffers) {
       mCommandBuffers.emplace_back(std::move(CommandBuffer(commandBuffer)));
    }
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