#include "CommandBuffer.h"

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
    info.level = level;
    info.commandBufferCount = 1;

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
    info.flags = usageFlags;

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

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = renderPass.vkRenderPass();
    info.framebuffer = frameBuffer;
    info.renderArea.offset = {0, 0};
    info.renderArea.extent = imageExtent;
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
                      const Semaphore& waitSemaphore,
                      const Semaphore& signalSemaphore,
                      const Fence& inFlightFence,
                      const VkPipelineStageFlags waitStage) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    assert(queue != VK_NULL_HANDLE);
    
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    // Set semaphore to wait on before execution begins and in which
    // stage of the pipeline to wait.
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &waitSemaphore.vkSemaphore();
    info.pWaitDstStageMask = &waitStage;

    // Set command buffer
    info.commandBufferCount = 1;
    info.pCommandBuffers = &mCommandBuffer;

    // Specify which semaphores to signal once the command 
    // buffer has finished execution.
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &signalSemaphore.vkSemaphore();

    vkChecker(vkQueueSubmit(queue,
                            1,
                            &info,
                            inFlightFence.vkFence()));
}
}