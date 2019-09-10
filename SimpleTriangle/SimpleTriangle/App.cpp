#include "App.h"

#include <cassert>

#include "utils/DebugUtils.h"
#include "utils/vertex/PosColorVertex.h"

using namespace vk;

App::App(const uint32_t windowWidth,
         const uint32_t windowHeight,
         const char* windowTitle,
         const vk::RenderPassCreator& renderPassCreator,
         const vk::GraphicsPipelineCreator& graphicsPipelineCreator)
    : mSystemManager(windowWidth, 
                     windowHeight, 
                     windowTitle)
    , mRenderPass(renderPassCreator(mSystemManager.logicalDevice(),
                                    mSystemManager.swapChain()))
    , mGraphicsPipeline(graphicsPipelineCreator(mSystemManager.logicalDevice(),
                                                mSystemManager.swapChain(),
                                                *mRenderPass))
    , mFrameBuffers(new FrameBuffers(mSystemManager.logicalDevice(),
                                     mSystemManager.swapChain(),
                                     *mRenderPass))
    , mCommandBuffers(new CommandBuffers(mSystemManager.logicalDevice(),
                                         mSystemManager.graphicsCommandPool(),
                                         mFrameBuffers->bufferCount(),
                                         VK_COMMAND_BUFFER_LEVEL_PRIMARY)) 
    , mImageAvailableSemaphores(mSystemManager.logicalDevice(), 
                                mFrameBuffers->bufferCount())
    , mRenderFinishedSemaphores(mSystemManager.logicalDevice(), 
                                mFrameBuffers->bufferCount())
    , mFences(mSystemManager.logicalDevice(),
              mFrameBuffers->bufferCount())
{
    createBuffers();
    recordCommandBuffers();
}

void 
App::run() {   
    while (glfwWindowShouldClose(&mSystemManager.window().glfwWindow()) == 0) {
        glfwPollEvents();
        submitCommandBufferAndPresent();
    }

    // We need to wait on the hsot for the completion of outstanding
    // operations for a given queue.
    // vkDeviceWaitIdle is equivalent to submitting fences to all
    // the queues owned yb the device a and waiting with an infinite 
    // timeout for these fences to signal.
    vkChecker(vkDeviceWaitIdle(mSystemManager.logicalDevice().vkDevice()));
}

void 
App::createBuffers() {
    assert(mVertexBuffer == nullptr);

    std::vector<vk::PosColorVertex> vertices
    {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    const size_t verticesSize = sizeof(PosColorVertex) * vertices.size();

    mVertexBuffer.reset(new Buffer(mSystemManager.logicalDevice(),
                                   verticesSize,
                                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                   VK_SHARING_MODE_EXCLUSIVE,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

    mVertexBuffer->copyToMemory(vertices.data(),
                                0,
                                verticesSize,
                                0);
}

void
App::recordCommandBuffers() {
    assert(mCommandBuffers != nullptr);
    assert(mFrameBuffers != nullptr);
    for (size_t i = 0; i < mCommandBuffers->bufferCount(); ++i) {
        CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(i);

        commandBuffer.beginRecording(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

        commandBuffer.beginPass(*mRenderPass,
                                mFrameBuffers->buffer(i),
                                mSystemManager.swapChain().imageExtent());

        commandBuffer.bindPipeline(*mGraphicsPipeline);

        commandBuffer.bindVertexBuffer(*mVertexBuffer);

        commandBuffer.draw(static_cast<uint32_t>(mVertexBuffer->size() / sizeof(PosColorVertex)));

        commandBuffer.endPass();

        commandBuffer.endRecording();
    }
}

void 
App::submitCommandBufferAndPresent() {
    assert(mCommandBuffers != nullptr);

    const Fence& fence = mFences.nextAvailableFence();
    fence.waitAndReset();

    Semaphore& imageAvailableSemaphore = mImageAvailableSemaphores.nextAvailableSemaphore();
    Semaphore& renderFinishedSemaphore = mRenderFinishedSemaphores.nextAvailableSemaphore();    

    const uint32_t swapChainImageIndex = mSystemManager.swapChain().acquireNextImage(imageAvailableSemaphore);
    assert(swapChainImageIndex < static_cast<uint32_t>(mCommandBuffers->bufferCount()));

    CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(swapChainImageIndex);
    commandBuffer.submit(mSystemManager.logicalDevice().graphicsQueue(),
                         imageAvailableSemaphore,
                         renderFinishedSemaphore,
                         fence,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    mSystemManager.swapChain().present(mSystemManager.logicalDevice().presentationQueue(),
                                       renderFinishedSemaphore,
                                       swapChainImageIndex);
}