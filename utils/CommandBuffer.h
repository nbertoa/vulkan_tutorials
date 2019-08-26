#ifndef UTILS_COMMAND_BUFFER
#define UTILS_COMMAND_BUFFER

#include <cassert>
#include <vulkan/vulkan.h>

namespace vk {
class CommandPool;
class Fence;
class GraphicsPipeline;
class LogicalDevice;
class RenderPass;
class Semaphore;

// VkCommandBuffer wrapper to be able to create and use it easily.
// Command buffers are objects used to record commands which can be
// subsequently submitted to a device queue for execution.
// There are two levels of command buffers: 
// - Primary command buffers which can execute secondary 
//   command buffers, and which are submitted to queues.
// - Secondary command buffers, which can be executed by 
//   primary command buffers, and which are not directly
//   submitted to queues.
class CommandBuffer {
public:
    CommandBuffer(const LogicalDevice& logicalDevice,
                  const CommandPool& commandPool,
                  const VkCommandBufferLevel level);

    CommandBuffer(const VkCommandBuffer commandBuffer);

    CommandBuffer(CommandBuffer&& other) noexcept;

    CommandBuffer(const CommandBuffer&) = delete;
    const CommandBuffer& operator=(const CommandBuffer&) = delete;

    void beginRecording(const VkCommandBufferUsageFlags usageFlags);
    void endRecording();

    void beginPass(const RenderPass& renderPass,
                   const VkFramebuffer frameBuffer,
                   const VkExtent2D& imageExtent);
    void endPass();

    void bindPipeline(const GraphicsPipeline& graphicsPipeline);

    void draw(const uint32_t vertexCount,
              const uint32_t instanceCount,
              const uint32_t firstVertex,
              const uint32_t firstInstance);

    void submit(const VkQueue queue,
                const Semaphore& waitSemaphore,
                const Semaphore& signalSemaphore,
                const Fence& inFlightFence,
                const VkPipelineStageFlags waitStage);

private:
    VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
};
}

#endif