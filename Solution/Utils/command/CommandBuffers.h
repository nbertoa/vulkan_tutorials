#ifndef UTILS_COMMAND_COMMAND_BUFFERS
#define UTILS_COMMAND_COMMAND_BUFFERS

#include <vector>
#include <vulkan/vulkan.hpp>

#include "CommandBuffer.h"

namespace vk2 {
class CommandPool;
class GraphicsPipeline;
class RenderPass;

class CommandBuffers {
public:
    // Read CommandBuffer to understand this
    CommandBuffers(const CommandPool& commandPool,
                   const uint32_t bufferCount,
                   const VkCommandBufferLevel level);
    CommandBuffers(CommandBuffers&& other) noexcept;
    CommandBuffers(const CommandBuffers&) = delete;
    const CommandBuffers& operator=(const CommandBuffers&) = delete;

    uint32_t 
    bufferCount() const;

    CommandBuffer& 
    commandBuffer(const uint32_t bufferIndex);

private:
    std::vector<CommandBuffer> mCommandBuffers;
};
}

#endif