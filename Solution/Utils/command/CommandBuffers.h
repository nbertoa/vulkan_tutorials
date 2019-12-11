#ifndef UTILS_COMMAND_COMMAND_BUFFERS
#define UTILS_COMMAND_COMMAND_BUFFERS

#include <vector>
#include <vulkan/vulkan.hpp>

#include "CommandBuffer.h"

namespace vk2 {

class CommandBuffers {
public:
    // Read CommandBuffer to understand this
    CommandBuffers(const vk::CommandPool commandPool,
                   const size_t bufferCount,
                   const vk::CommandBufferLevel level);
    CommandBuffers(const CommandBuffers&) = delete;
    const CommandBuffers& operator=(const CommandBuffers&) = delete;

    size_t
    bufferCount() const;

    CommandBuffer& 
    commandBuffer(const size_t bufferIndex);

private:
    std::vector<CommandBuffer> mCommandBuffers;
};
}

#endif