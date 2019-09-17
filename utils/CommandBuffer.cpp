#include "CommandBuffer.h"

#include "Buffer.h"
#include "CommandPool.h"
#include "DebugUtils.h"
#include "Fence.h"
#include "GraphicsPipeline.h"
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "Semaphore.h"

namespace vk {
CommandBuffer::CommandBuffer(const LogicalDevice& logicalDevice,
                             const CommandPool& commandPool,
                             const VkCommandBufferLevel level) {

    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = commandPool.vkCommandPool();
    info.commandBufferCount = 1;

    // Specify if the allocated command buffers are primary
    // or secondary command buffers.
    //
    // - VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted
    // to a queue for execution, but cannot be called
    // from other command buffers.
    //
    // - VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted
    // directly, but can be called from primary command
    // buffers.
    info.level = level;   

    vkChecker(vkAllocateCommandBuffers(logicalDevice.vkDevice(),
                                       &info,
                                       &mCommandBuffer));
}

CommandBuffer::CommandBuffer(const VkCommandBuffer commandBuffer) 
    : mCommandBuffer(commandBuffer)
{
    assert(mCommandBuffer != VK_NULL_HANDLE);
}
CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept 
    : mCommandBuffer(other.mCommandBuffer)
{
    other.mCommandBuffer = VK_NULL_HANDLE;
}

void
CommandBuffer::beginRecording(const VkCommandBufferUsageFlags usageFlags) {
    assert(mCommandBuffer != VK_NULL_HANDLE);

    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

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
    info.flags = usageFlags;

    // This is only relevant for secondary command buffers
    // It specifies which state to inherit from the calling
    // primary command buffers.
    info.pInheritanceInfo = nullptr;

    vkChecker(vkBeginCommandBuffer(mCommandBuffer,
                                   &info));
}

void
CommandBuffer::endRecording() {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    vkChecker(vkEndCommandBuffer(mCommandBuffer));
}

void
CommandBuffer::beginPass(const RenderPass& renderPass,
                         const VkFramebuffer frameBuffer,
                         const VkExtent2D& imageExtent) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    assert(frameBuffer != VK_NULL_HANDLE);

    

    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = renderPass.vkRenderPass();
    info.framebuffer = frameBuffer;
    // Size of the render area which defines where shader
    // loads and stores will take place. The pixels outside
    // this region will have undefined values.
    info.renderArea.offset = {0, 0};
    info.renderArea.extent = imageExtent;

    // Define the clear values to use for
    // VK_ATTACHMENT_LOAD_OP_CLEAR, which we need as
    // load operation for the color attachment.
    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    info.clearValueCount = 1;
    info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(mCommandBuffer,
                         &info,
                         VK_SUBPASS_CONTENTS_INLINE);
}

void
CommandBuffer::endPass() {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    vkCmdEndRenderPass(mCommandBuffer);
}

void
CommandBuffer::bindPipeline(const GraphicsPipeline& graphicsPipeline) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    vkCmdBindPipeline(mCommandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipeline.vkPipeline());
}

void 
CommandBuffer::bindVertexBuffer(const Buffer& buffer) {
    assert(mCommandBuffer != VK_NULL_HANDLE);

    VkBuffer buffers[] = { buffer.vkBuffer() };
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(mCommandBuffer,
                           0, // first vertex buffer to bind
                           1, // number of vertex buffers to bind
                           buffers,
                           offsets); // Byte offsets to start reading vertex data from
}

void 
CommandBuffer::copyBuffer(const Buffer& sourceBuffer,
                          const Buffer& destinationBuffer,
                          const VkBufferCopy& bufferCopy) {
    assert(mCommandBuffer != VK_NULL_HANDLE);

    vkCmdCopyBuffer(mCommandBuffer,
                    sourceBuffer.vkBuffer(),
                    destinationBuffer.vkBuffer(),
                    1,
                    &bufferCopy);
}

void
CommandBuffer::draw(const uint32_t vertexCount,
                    const uint32_t instanceCount,
                    const uint32_t firstVertex,
                    const uint32_t firstInstance) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    vkCmdDraw(mCommandBuffer,
              vertexCount,
              instanceCount,
              firstVertex,
              firstInstance);
}

void 
CommandBuffer::submit(const VkQueue queue, 
                      const Semaphore* waitSemaphore,
                      const Semaphore* signalSemaphore,
                      const Fence& inFlightFence,
                      const VkPipelineStageFlags waitStage) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    assert(queue != VK_NULL_HANDLE);
    
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    // Set semaphore to wait on before execution begins and in which
    // stage of the pipeline to wait.
    info.waitSemaphoreCount = waitSemaphore != nullptr ? 1 : 0;
    info.pWaitSemaphores = waitSemaphore != nullptr ? &waitSemaphore->vkSemaphore() : nullptr;
    info.pWaitDstStageMask = &waitStage;

    // Set command buffer
    info.commandBufferCount = 1;
    info.pCommandBuffers = &mCommandBuffer;

    // Specify which semaphores to signal once the command 
    // buffer has finished execution.
    info.signalSemaphoreCount = signalSemaphore != nullptr ? 1 : 0;
    info.pSignalSemaphores = signalSemaphore != nullptr ? &signalSemaphore->vkSemaphore() : nullptr;

    vkChecker(vkQueueSubmit(queue,
                            1,
                            &info,
                            inFlightFence.vkFence()));
}
}