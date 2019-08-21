#ifndef UTILS_COMMAND_BUFFERS
#define UTILS_COMMAND_BUFFERS

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class CommandPool;
class GraphicsPipeline;
class LogicalDevice;
class RenderPass;

// VkCommandBuffer list wrapper to be able to create and record them easily.
// Command buffers are objects used to record commands which can be
// subsequently submitted to a device queue for execution.
// There are two levels of command buffers: 
// - Primary command buffers which can execute secondary 
//   command buffers, and which are submitted to queues.
// - Secondary command buffers, which can be executed by 
//   primary command buffers, and which are not directly
//   submitted to queues.
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

    void beginRecording(const size_t bufferIndex,
                        const VkCommandBufferUsageFlags usageFlags);
    void endRecording(const size_t bufferIndex);

    void beginPass(const size_t bufferIndex,
                   const RenderPass& renderPass,
                   const VkFramebuffer frameBuffer,
                   const VkExtent2D& imageExtent);
    void endPass(const size_t bufferIndex);

    void bindPipeline(const size_t bufferIndex, 
                      const GraphicsPipeline& graphicsPipeline);

    void draw(const size_t bufferIndex,
              const uint32_t vertexCount,
              const uint32_t instanceCount,
              const uint32_t firstVertex,
              const uint32_t firstInstance);

private:
    std::vector<VkCommandBuffer> mCommandBuffers;
};
}

#endif