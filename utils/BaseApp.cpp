#include "BaseApp.h"

#include <cassert>

#include "utils/DebugUtils.h"

namespace vk {
BaseApp::BaseApp(const uint32_t windowWidth,
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
              mFrameBuffers->bufferCount()) {}

void
BaseApp::run() {
    while (glfwWindowShouldClose(&mSystemManager.window().glfwWindow()) == 0) {
        glfwPollEvents();

        Semaphore& imageAvailableSemaphore = mImageAvailableSemaphores.nextAvailableSemaphore();
        mSystemManager.swapChain().acquireNextImage(imageAvailableSemaphore);

        processCurrentFrame();

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
BaseApp::submitCommandBufferAndPresent() {
    assert(mCommandBuffers != nullptr);

    const Fence& fence = mFences.nextAvailableFence();
    fence.waitAndReset();

    // This semaphore was already obtained in run()
    Semaphore& imageAvailableSemaphore = mImageAvailableSemaphores.currentSemaphore();

    Semaphore& renderFinishedSemaphore = mRenderFinishedSemaphores.nextAvailableSemaphore();

    // The next image was already obtained in run()
    const uint32_t swapChainImageIndex = mSystemManager.swapChain().currentImageIndex();
    assert(swapChainImageIndex < static_cast<uint32_t>(mCommandBuffers->bufferCount()));

    CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(swapChainImageIndex);
    commandBuffer.submit(mSystemManager.logicalDevice().graphicsQueue(),
                         &imageAvailableSemaphore,
                         &renderFinishedSemaphore,
                         fence,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    mSystemManager.swapChain().present(renderFinishedSemaphore,
                                       swapChainImageIndex);
}
}