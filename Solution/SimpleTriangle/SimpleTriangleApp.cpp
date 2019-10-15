#include "SimpleTriangleApp.h"

#include <cassert>

#include "Utils/vertex/PosColorVertex.h"

using namespace vk;

SimpleTriangleApp::SimpleTriangleApp(const uint32_t windowWidth,
                                     const uint32_t windowHeight,
                                     const char* windowTitle,
                                     const vk::RenderPassCreator& renderPassCreator,
                                     const vk::GraphicsPipelineCreator& graphicsPipelineCreator)
    : BaseApp(windowWidth,
              windowHeight,
              windowTitle,
              renderPassCreator,
              graphicsPipelineCreator)
{
    createBuffers();
    recordCommandBuffers();
}

void 
SimpleTriangleApp::createBuffers() {
    assert(mGpuVertexBuffer == nullptr);

    std::vector<vk::PosColorVertex> vertices
    {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    const size_t verticesSize = sizeof(PosColorVertex) * vertices.size();

    Buffer cpuVertexBuffer(mSystemManager.logicalDevice(),
                           mSystemManager.physicalDevice(),
                           verticesSize,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_SHARING_MODE_EXCLUSIVE,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    cpuVertexBuffer.copyToHostMemory(vertices.data(),
                                     verticesSize,
                                     0);

    mGpuVertexBuffer.reset(new Buffer(mSystemManager.logicalDevice(),
                                      mSystemManager.physicalDevice(),
                                      verticesSize,
                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      VK_SHARING_MODE_EXCLUSIVE,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    mGpuVertexBuffer->copyFromBufferToDeviceMemory(cpuVertexBuffer,
                                                   mSystemManager.transferCommandPool());
}

void
SimpleTriangleApp::recordCommandBuffers() {
    assert(mCommandBuffers != nullptr);
    assert(mFrameBuffers != nullptr);
    for (size_t i = 0; i < mCommandBuffers->bufferCount(); ++i) {
        CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(i);

        commandBuffer.beginRecording(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

        commandBuffer.beginPass(*mRenderPass,
                                mFrameBuffers->buffer(i),
                                mSystemManager.swapChain().imageExtent());

        commandBuffer.bindPipeline(*mGraphicsPipeline);

        commandBuffer.bindVertexBuffer(*mGpuVertexBuffer);

        commandBuffer.draw(static_cast<uint32_t>(mGpuVertexBuffer->size() / sizeof(PosColorVertex)));

        commandBuffer.endPass();

        commandBuffer.endRecording();
    }
}