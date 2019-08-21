#include "CommandBuffers.h"

#include "CommandPool.h"
#include "DebugUtils.h"
#include "GraphicsPipeline.h"
#include "LogicalDevice.h"
#include "RenderPass.h"

namespace vk {
CommandBuffers::CommandBuffers(const LogicalDevice& logicalDevice,
                               const CommandPool& commandPool,
                               const size_t bufferCount,
                               const VkCommandBufferLevel level)
{
    assert(bufferCount > 0);
    mCommandBuffers.resize(bufferCount);

    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = commandPool.vkCommandPool();
    info.level = level;
    info.commandBufferCount = static_cast<uint32_t>(bufferCount);

    vkChecker(vkAllocateCommandBuffers(logicalDevice.vkDevice(),
                                       &info,
                                       mCommandBuffers.data()));
}

void 
CommandBuffers::beginRecording(const size_t bufferIndex,
                               const VkCommandBufferUsageFlags usageFlags) {
    assert(bufferIndex < mCommandBuffers.size());

    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = usageFlags;

    vkChecker(vkBeginCommandBuffer(mCommandBuffers[bufferIndex],
                                   &info));
}

void
CommandBuffers::endRecording(const size_t bufferIndex) {
    assert(bufferIndex < mCommandBuffers.size());

    vkChecker(vkEndCommandBuffer(mCommandBuffers[bufferIndex]));
}

void 
CommandBuffers::beginPass(const size_t bufferIndex,
                          const RenderPass& renderPass,
                          const VkFramebuffer frameBuffer,
                          const VkExtent2D& imageExtent) {
    assert(bufferIndex < mCommandBuffers.size());
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

    vkCmdBeginRenderPass(mCommandBuffers[bufferIndex],
                         &info,
                         VK_SUBPASS_CONTENTS_INLINE);
}

void 
CommandBuffers::endPass(const size_t bufferIndex) {
    assert(bufferIndex < mCommandBuffers.size());
    vkCmdEndRenderPass(mCommandBuffers[bufferIndex]);
}

void
CommandBuffers::bindPipeline(const size_t bufferIndex,
                             const GraphicsPipeline& graphicsPipeline) {
    assert(bufferIndex < mCommandBuffers.size());
    vkCmdBindPipeline(mCommandBuffers[bufferIndex],
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipeline.vkPipeline());
}

void 
CommandBuffers::draw(const size_t bufferIndex,
                     const uint32_t vertexCount,
                     const uint32_t instanceCount,
                     const uint32_t firstVertex,
                     const uint32_t firstInstance) {
    assert(bufferIndex < mCommandBuffers.size());
    vkCmdDraw(mCommandBuffers[bufferIndex],
              vertexCount,
              instanceCount,
              firstVertex,
              firstInstance);
}
}