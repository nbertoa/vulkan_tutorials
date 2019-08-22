#ifndef UTILS_COMMAND_BUFFERS
#define UTILS_COMMAND_BUFFERS

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

#include "CommandBuffer.h"

namespace vk {
class CommandPool;
class GraphicsPipeline;
class LogicalDevice;
class RenderPass;

// VkCommandBuffer list wrapper to be able to 
// create and record them easily.
class CommandBuffers {
public:
    CommandBuffers(const LogicalDevice& logicalDevice,
                   const CommandPool& commandPool,
                   const size_t bufferCount,
                   const VkCommandBufferLevel level);

    size_t bufferCount() const {
        assert(mCommandBuffers.empty() == false);
        return mCommandBuffers.size();
    }

    CommandBuffer& commandBuffer(const size_t bufferIndex) {
        assert(bufferIndex < mCommandBuffers.size());
        return mCommandBuffers[bufferIndex];
    }

private:
    std::vector<CommandBuffer> mCommandBuffers;
};
}

#endif