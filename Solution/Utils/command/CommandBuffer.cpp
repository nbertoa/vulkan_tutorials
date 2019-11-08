#include "CommandBuffer.h"

#include <cassert>

#include "CommandPool.h"
#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"
#include "../pipeline/GraphicsPipeline.h"
#include "../render_pass/RenderPass.h"
#include "../resource/Buffer.h"
#include "../resource/Image.h"
#include "../resource/ImageMemoryBarrier.h"
#include "../sync/Fence.h"
#include "../sync/Semaphore.h"

namespace vk {
CommandBuffer::CommandBuffer(const LogicalDevice& logicalDevice,
                             const CommandPool& commandPool,
                             const VkCommandBufferLevel level) {
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

CommandBuffer
CommandBuffer::createAndBeginOneTimeSubmitCommandBuffer(const LogicalDevice& logicalDevice,
                                                        const CommandPool& commandPool) {
    CommandBuffer commandBuffer(logicalDevice,
                                commandPool,
                                VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    commandBuffer.beginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    return commandBuffer;
}

void
CommandBuffer::beginRecording(const VkCommandBufferUsageFlags usageFlags) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    
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
                         const VkExtent2D& imageExtent,
                         const std::vector<VkClearValue>& clearValues) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    assert(frameBuffer != VK_NULL_HANDLE);     
    assert(clearValues.empty() == false);

    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = renderPass.vkRenderPass();
    info.framebuffer = frameBuffer;
    info.renderArea.offset = {0, 0};
    info.renderArea.extent = imageExtent;
    info.clearValueCount = static_cast<uint32_t>(clearValues.size());
    info.pClearValues = clearValues.data();

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
CommandBuffer::bindDescriptorSet(const PipelineLayout& pipelineLayout,
                                 const VkDescriptorSet descriptorSet) {
    assert(descriptorSet != VK_NULL_HANDLE);
    vkCmdBindDescriptorSets(mCommandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout.vkPipelineLayout(),
                            0,
                            1,
                            &descriptorSet,
                            0,
                            nullptr);
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
CommandBuffer::copyBufferToImage(const Buffer& sourceBuffer,
                                 const Image& destinationImage,
                                 const VkBufferImageCopy& regionToCopy,
                                 const VkImageLayout destImageLayout) {
    assert(mCommandBuffer != VK_NULL_HANDLE);

    vkCmdCopyBufferToImage(mCommandBuffer,
                           sourceBuffer.vkBuffer(),
                           destinationImage.vkImage(),
                           destImageLayout,
                           1, // number of regions to copy
                           &regionToCopy);
}

void
CommandBuffer::imagePipelineBarrier(const ImageMemoryBarrier& imageMemoryBarrier,
                                    const VkPipelineStageFlags sourceStageMask,
                                    const VkPipelineStageFlags destStageMask,
                                    const VkDependencyFlags dependencyFlags) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    vkCmdPipelineBarrier(mCommandBuffer,
                         sourceStageMask,
                         destStageMask,
                         dependencyFlags,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &imageMemoryBarrier.vkMemoryBarrier());
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
    // - waitSemaphoreCount upon which to wait 
    //   before executing the command buffers for the batch.
    // - pWaitSemaphores upon which to wait before the command buffers for this batch begin execution.
    //   If semaphores to wait on are provided, they define a semaphore wait operation.
    // - pWaitDstStageMask array at which each corresponding semaphore wait will occur.
    // - commandBufferCount to execute in the batch.
    // - pCommandBuffers to execute in the batch.
    // - signalSemaphoreCount to be signaled once the 
    //   commands specified in pCommandBuffers have completed execution.
    // - pSignalSemaphores which will be signaled when the command buffers 
    //   for this batch have completed execution.
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