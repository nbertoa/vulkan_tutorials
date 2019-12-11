#include "CommandBuffer.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"
#include "../pipeline/GraphicsPipeline.h"
#include "../resource/Buffer.h"
#include "../resource/Image.h"
#include "../resource/ImageMemoryBarrier.h"

namespace vk2 {
CommandBuffer::CommandBuffer(const vk::CommandPool commandPool,
                             const vk::CommandBufferLevel level) {

    vk::CommandBufferAllocateInfo info;
    info.setCommandBufferCount(1);
    info.setCommandPool(commandPool);
    info.setLevel(level);

    std::vector<vk::CommandBuffer> buffers = LogicalDevice::device().allocateCommandBuffers(info);
    assert(buffers.size() == 1);
    mCommandBuffer = buffers.front();
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept 
    : mCommandBuffer(other.mCommandBuffer)
{
    other.mCommandBuffer = vk::CommandBuffer();
}

CommandBuffer::CommandBuffer(const vk::CommandBuffer commandBuffer) 
    : mCommandBuffer(commandBuffer)
{
    assert(commandBuffer != VK_NULL_HANDLE);
}

void
CommandBuffer::beginRecording(const vk::CommandBufferUsageFlagBits usageFlags) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    
    vk::CommandBufferBeginInfo info;
    info.setFlags(usageFlags);
    vkChecker(mCommandBuffer.begin(&info));
}

void
CommandBuffer::endRecording() {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    mCommandBuffer.end();
}

void
CommandBuffer::beginPass(const vk::RenderPass renderPass,
                         const vk::Framebuffer frameBuffer,
                         const vk::Extent2D& imageExtent) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    assert(frameBuffer != VK_NULL_HANDLE);   

    const vk::ClearColorValue colorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
    const vk::ClearValue clearValue(colorValue);

    vk::RenderPassBeginInfo info;
    info.setRenderArea(vk::Rect2D {vk::Offset2D {0, 0}, imageExtent});
    info.setFramebuffer(frameBuffer);
    info.setClearValueCount(1);
    info.setPClearValues(&clearValue);
    info.setRenderPass(renderPass);

    mCommandBuffer.beginRenderPass(info,
                                   vk::SubpassContents::eInline);
}

void
CommandBuffer::endPass() {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    mCommandBuffer.endRenderPass();
}

void
CommandBuffer::bindPipeline(const GraphicsPipeline& graphicsPipeline) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                graphicsPipeline.pipeline());
}

void
CommandBuffer::bindDescriptorSet(const vk::PipelineLayout pipelineLayout,
                                 const vk::DescriptorSet descriptorSet) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    assert(descriptorSet != VK_NULL_HANDLE);
    assert(pipelineLayout != VK_NULL_HANDLE);

    mCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                      pipelineLayout,
                                      0, // first descriptor set
                                      {descriptorSet},
                                      {}); // dynamic arrays
}

void 
CommandBuffer::bindVertexBuffer(const Buffer& buffer) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    mCommandBuffer.bindVertexBuffers(0, // first vertex buffer to bind
                                     {buffer.vkBuffer()},
                                     {0}); // offsets 
}

void
CommandBuffer::bindIndexBuffer(const Buffer& buffer,
                               const vk::IndexType indexType) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    mCommandBuffer.bindIndexBuffer(buffer.vkBuffer(),
                                   0, // offset
                                   indexType);
}

void 
CommandBuffer::copyBuffer(const Buffer& sourceBuffer,
                          const Buffer& destinationBuffer,
                          const vk::BufferCopy& regionToCopy) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    mCommandBuffer.copyBuffer(sourceBuffer.vkBuffer(),
                              destinationBuffer.vkBuffer(),
                              {regionToCopy});
}

void
CommandBuffer::copyBufferToImage(const Buffer& sourceBuffer,
                                 const Image& destinationImage,
                                 const vk::BufferImageCopy& regionToCopy,
                                 const vk::ImageLayout destImageLayout) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    mCommandBuffer.copyBufferToImage(sourceBuffer.vkBuffer(),
                                     destinationImage.vkImage(),
                                     destImageLayout,
                                     {regionToCopy});
}

void
CommandBuffer::imagePipelineBarrier(const ImageMemoryBarrier& imageMemoryBarrier,
                                    const vk::PipelineStageFlagBits sourceStageMask,
                                    const vk::PipelineStageFlagBits destStageMask,
                                    const vk::DependencyFlagBits dependencyFlags) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    mCommandBuffer.pipelineBarrier(sourceStageMask,
                                   destStageMask,
                                   dependencyFlags,
                                   {}, // memory barriers
                                   {}, // buffer memory barriers
                                   {imageMemoryBarrier.vkMemoryBarrier()});
}

void
CommandBuffer::draw(const uint32_t vertexCount,
                    const uint32_t instanceCount,
                    const uint32_t firstVertex,
                    const uint32_t firstInstance) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    mCommandBuffer.draw(vertexCount,
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
    mCommandBuffer.drawIndexed(indexCount,
                               instanceCount,
                               firstIndex,
                               vertexOffset,
                               firstInstance);
}

void 
CommandBuffer::submit(const vk::Queue queue, 
                      const vk::Semaphore* waitSemaphore,
                      const vk::Semaphore* signalSemaphore,
                      const vk::Fence& executionCompletedFence,
                      const vk::PipelineStageFlagBits waitStageFlags) {
    assert(mCommandBuffer != VK_NULL_HANDLE);
    assert(queue != VK_NULL_HANDLE);
    
    vk::SubmitInfo info;
    info.setWaitSemaphoreCount(waitSemaphore != nullptr ? 1 : 0);
    info.setPWaitSemaphores(waitSemaphore);
    info.setSignalSemaphoreCount(signalSemaphore != nullptr ? 1 : 0);
    info.setPSignalSemaphores(signalSemaphore);
    info.setCommandBufferCount(1);
    info.setPCommandBuffers(&mCommandBuffer);
    const vk::PipelineStageFlags flags(waitStageFlags);
    info.setPWaitDstStageMask(&flags);
    
    queue.submit({info},
                 executionCompletedFence);
}
}