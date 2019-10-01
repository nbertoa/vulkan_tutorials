#include "SimpleTriangleWithIndexBufferApp.h"

#include <cassert>

#include "Utils/vertex/PosColorVertex.h"

using namespace vk;

SimpleTriangleWithIndexBufferApp::SimpleTriangleWithIndexBufferApp(const uint32_t windowWidth,
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
SimpleTriangleWithIndexBufferApp::createBuffers() {
    createVertexBuffer();
    createIndexBuffer();
}

void 
SimpleTriangleWithIndexBufferApp::createVertexBuffer() {
    assert(mGpuVertexBuffer == nullptr);

    std::vector<PosColorVertex> screenSpaceVertices
    {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
    };

    const size_t verticesSize = sizeof(PosColorVertex) * screenSpaceVertices.size();

    Buffer cpuVertexBuffer(mSystemManager.logicalDevice(),
                           verticesSize,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_SHARING_MODE_EXCLUSIVE,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    cpuVertexBuffer.copyToHostMemory(screenSpaceVertices.data(),
                                     verticesSize,
                                     0);

    mGpuVertexBuffer.reset(new Buffer(mSystemManager.logicalDevice(),
                                      verticesSize,
                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      VK_SHARING_MODE_EXCLUSIVE,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    mGpuVertexBuffer->copyFromBufferToDeviceMemory(cpuVertexBuffer,
                                                   mSystemManager.transferCommandPool());
}

void 
SimpleTriangleWithIndexBufferApp::createIndexBuffer() {
    assert(mGpuIndexBuffer == nullptr);

    std::vector<uint32_t> indices
    {
        0, 1, 2, // upper-right triangle
        2, 3, 0, // bottom-left triangle
    };

    const size_t indicesSize = sizeof(uint32_t) * indices.size();

    Buffer cpuIndexBuffer(mSystemManager.logicalDevice(),
                          indicesSize,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_SHARING_MODE_EXCLUSIVE,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    cpuIndexBuffer.copyToHostMemory(indices.data(),
                                    indicesSize,
                                    0);

    mGpuIndexBuffer.reset(new Buffer(mSystemManager.logicalDevice(),
                                     indicesSize,
                                     VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                     VK_SHARING_MODE_EXCLUSIVE,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    mGpuIndexBuffer->copyFromBufferToDeviceMemory(cpuIndexBuffer,
                                                  mSystemManager.transferCommandPool());
}

void
SimpleTriangleWithIndexBufferApp::recordCommandBuffers() {
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

        commandBuffer.bindIndexBuffer(*mGpuIndexBuffer,
                                      VK_INDEX_TYPE_UINT32);

        commandBuffer.drawIndexed(static_cast<uint32_t>(mGpuIndexBuffer->size() / sizeof(uint32_t)));

        commandBuffer.endPass();

        commandBuffer.endRecording();
    }
}