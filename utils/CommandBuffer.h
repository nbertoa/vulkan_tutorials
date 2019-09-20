#ifndef UTILS_COMMAND_BUFFER
#define UTILS_COMMAND_BUFFER

#include <vulkan/vulkan.h>

namespace vk {
class Buffer;
class CommandPool;
class Fence;
class GraphicsPipeline;
class LogicalDevice;
class RenderPass;
class Semaphore;

// VkCommandBuffer wrapper.
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
    // Level:
    // - VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted
    // to a queue for execution, but cannot be called
    // from other command buffers.
    //
    // - VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted
    // directly, but can be called from primary command
    // buffers.
    CommandBuffer(const LogicalDevice& logicalDevice,
                  const CommandPool& commandPool,
                  const VkCommandBufferLevel level);
    CommandBuffer(const VkCommandBuffer commandBuffer);
    CommandBuffer(CommandBuffer&& other) noexcept;
    CommandBuffer(const CommandBuffer&) = delete;
    const CommandBuffer& operator=(const CommandBuffer&) = delete;

    // Command Buffer Usage Flags:
    // Specify how we are going to use the command buffer. The 
    // following values are available:
    // - VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer
    // will be rerecorded right after executing it once.
    //
    // - VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a 
    // secondary command buffer that will be entirely within a single
    // render pass.
    //
    // - VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command
    // buffer can be resubmitted while it is also already pending 
    // execution.
    void beginRecording(const VkCommandBufferUsageFlags usageFlags);
    void endRecording();

    void beginPass(const RenderPass& renderPass,
                   const VkFramebuffer frameBuffer,
                   const VkExtent2D& imageExtent);
    void endPass();

    void bindPipeline(const GraphicsPipeline& graphicsPipeline);

    void bindVertexBuffer(const Buffer& buffer);

    void bindIndexBuffer(const Buffer& buffer,
                         const VkIndexType indexType);

    void copyBuffer(const Buffer& sourceBuffer,
                    const Buffer& destinationBuffer,
                    const VkBufferCopy& bufferCopy);

    void draw(const uint32_t vertexCount,
              const uint32_t instanceCount = 1,
              const uint32_t firstVertex = 0,
              const uint32_t firstInstance = 0);

    void drawIndexed(const uint32_t vertexCount,
                     const uint32_t indexCount,
                     const uint32_t instanceCount = 1,
                     const uint32_t firstIndex = 0,
                     const uint32_t vertexOffset = 0,
                     const uint32_t firstInstance = 0);

    void submit(const VkQueue queue,
                const Semaphore* waitSemaphore,
                const Semaphore* signalSemaphore,
                const Fence& inFlightFence,
                const VkPipelineStageFlags waitStage);

private:
    VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
};
}

#endif