#include "CommandBuffer.h"

#include <cassert>

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

    // VkCommandBufferAllocateInfo:
    // - commandPool is the command pool from which the command buffers are allocated.
    // - level is a VkCommandBufferLevel value specifying the command buffer level.
    // - commandBufferCount is the number of command buffers to allocate from the pool.
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = commandPool.vkCommandPool();
    info.commandBufferCount = 1;
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

    // VkCommandBufferBeginInfo:
    // - flags is a bitmask of VkCommandBufferUsageFlagBits specifying 
    //   usage behavior for the command buffer.
    // - pInheritanceInfo is a pointer to a VkCommandBufferInheritanceInfo structure, 
    //   used if commandBuffer is a secondary command buffer.If this is a primary 
    //   command buffer, then this value is ignored.
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = usageFlags;
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

    // VkRenderPassBeginInfo:
    // - renderPass is the render pass to begin an instance of.
    // - framebuffer is the framebuffer containing the attachments 
    //   that are used with the render pass.
    // - renderArea is the render area that is affected by the render 
    //   pass instance.
    // - clearValueCount is the number of elements in pClearValues.
    // - pClearValues is a pointer to an array of clearValueCount VkClearValue 
    //   structures that contains clear values for each attachment, if the attachment 
    //   uses a loadOp value of VK_ATTACHMENT_LOAD_OP_CLEAR or if the attachment has a depth/stencil 
    //   format and uses a stencilLoadOp value of VK_ATTACHMENT_LOAD_OP_CLEAR.
    //   The array is indexed by attachment number.
    //   Only elements corresponding to cleared attachments are used.
    //   Other elements of pClearValues are ignored.
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = renderPass.vkRenderPass();
    info.framebuffer = frameBuffer;
    info.renderArea.offset = {0, 0};
    info.renderArea.extent = imageExtent;
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
CommandBuffer::bindIndexBuffer(const Buffer& buffer,
                               const VkIndexType indexType) {
    assert(mCommandBuffer != VK_NULL_HANDLE);

    vkCmdBindIndexBuffer(mCommandBuffer,
                         buffer.vkBuffer(),
                         0, // Byte offset to start reading index data from
                         indexType);
}

void 
CommandBuffer::copyBuffer(const Buffer& sourceBuffer,
                          const Buffer& destinationBuffer,
                          const VkBufferCopy& regionToCopy) {
    assert(mCommandBuffer != VK_NULL_HANDLE);

    vkCmdCopyBuffer(mCommandBuffer,
                    sourceBuffer.vkBuffer(),
                    destinationBuffer.vkBuffer(),
                    1,
                    &regionToCopy);
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
CommandBuffer::drawIndexed(const uint32_t indexCount,
                           const uint32_t instanceCount,
                           const uint32_t firstIndex,
                           const uint32_t vertexOffset,
                           const uint32_t firstInstance) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    vkCmdDrawIndexed(mCommandBuffer,
                     indexCount,
                     instanceCount,
                     firstIndex,
                     vertexOffset,
                     firstInstance);
}

void 
CommandBuffer::submit(const VkQueue queue, 
                      const Semaphore* waitSemaphore,
                      const Semaphore* signalSemaphore,
                      const Fence& executionCompletedFence,
                      const VkPipelineStageFlags waitStageFlags) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    assert(queue != VK_NULL_HANDLE);
    
    // VkSubmitInfo:
    // - waitSemaphoreCount is the number of semaphores upon which to wait 
    //   before executing the command buffers for the batch.
    // - pWaitSemaphores is a pointer to an array of VkSemaphore handles upon 
    //   which to wait before the command buffers for this batch begin execution.
    //   If semaphores to wait on are provided, they define a semaphore wait operation.
    // - pWaitDstStageMask is a pointer to an array of pipeline stages at which 
    //   each corresponding semaphore wait will occur.
    // - commandBufferCount is the number of command buffers to execute in the batch.
    // - pCommandBuffers is a pointer to an array of VkCommandBuffer handles 
    //   to execute in the batch.
    // - signalSemaphoreCount is the number of semaphores to be signaled once the 
    //   commands specified in pCommandBuffers have completed execution.
    // - pSignalSemaphores is a pointer to an array of VkSemaphore handles which 
    //   will be signaled when the command buffers for this batch have completed execution.
    //   If semaphores to be signaled are provided, they define a semaphore signal operation.
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = waitSemaphore != nullptr ? 1 : 0;
    info.pWaitSemaphores = waitSemaphore != nullptr ? &waitSemaphore->vkSemaphore() : nullptr;
    info.pWaitDstStageMask = &waitStageFlags;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &mCommandBuffer;
    info.signalSemaphoreCount = signalSemaphore != nullptr ? 1 : 0;
    info.pSignalSemaphores = signalSemaphore != nullptr ? &signalSemaphore->vkSemaphore() : nullptr;

    vkChecker(vkQueueSubmit(queue,
                            1,
                            &info,
                            executionCompletedFence.vkFence()));
}
}