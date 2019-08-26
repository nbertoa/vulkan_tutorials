#include "App.h"

#include <cassert>

#include "utils/DebugUtils.h"
#include "utils/PipelineStateFactory.h"

using namespace vk;

App::App(const uint32_t windowWidth,
         const uint32_t windowHeight,
         const char* windowTitle)
    : mSystemManager(windowWidth, 
                     windowHeight, 
                     windowTitle)
    , mRenderPass(createRenderPass())
    , mGraphicsPipeline(createPipeline())
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

RenderPass*
App::createRenderPass() const {
    // Create attachment descriptions
    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.format = mSystemManager.swapChain().imageFormat();
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Create sub pass descriptions
    VkAttachmentReference attachmentReference = {};
    attachmentReference.attachment = 0;
    attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &attachmentReference;

    // Create subpass dependencies
    VkSubpassDependency dependency = {};
    // VK_SUBPASS_EXTERNAL: Implicit subpass before or after the render pass 
    // depending on whether it is specified in srcSubpass or dstSubpass.
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL; 
    dependency.dstSubpass = 0; // Our subpass (we have only 1)

    // Specify the operations to wait on and the stages in which these operations occur.
    // We need to wait for the swap chain to finish reading from the image
    // before we can access it.
    // This can be accomplished by waiting on the color attachment output stage itself.
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;

    // The operations that should wait on this are in the color attachment stage
    // adn involve the reading and writing of the color attachment.
    // These settings will prevent the transition from happening until it is
    // actually necessary (and allowed): when we want to start writing colors
    // to it.
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                               VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    return new RenderPass(mSystemManager.logicalDevice(),
                          {attachmentDescription},
                          {subpassDescription},
                          {dependency});
}

GraphicsPipeline*
App::createPipeline() {

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = PipelineStateFactory::emptyVertexInputState();
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo =
        PipelineStateFactory::createInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                       VK_FALSE);
    VkPipelineViewportStateCreateInfo viewportCreateInfo = mSystemManager.swapChain().pipelineViewportCreateInfo();
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = PipelineStateFactory::defaultRasterizationState();
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = PipelineStateFactory::disableMultisampleState();

    const VkPipelineColorBlendAttachmentState colorBlendAttachmentState = PipelineStateFactory::enableColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = PipelineStateFactory::colorBlendState(colorBlendAttachmentState);

    const ShaderModule vertexShaderModule(mSystemManager.logicalDevice(),
                                          "../../SimpleTriangle/resources/shaders/vert.spv",
                                          VK_SHADER_STAGE_VERTEX_BIT);

    const ShaderModule fragmentShaderModule(mSystemManager.logicalDevice(),
                                            "../../SimpleTriangle/resources/shaders/frag.spv",
                                            VK_SHADER_STAGE_FRAGMENT_BIT);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos =
    {
        vertexShaderModule.pipelineShaderStageCreateInfo(),
        fragmentShaderModule.pipelineShaderStageCreateInfo(),
    };

    std::unique_ptr<PipelineLayout> pipelineLayout(new PipelineLayout(mSystemManager.logicalDevice()));
    return new GraphicsPipeline(mSystemManager.logicalDevice(),
                                *mRenderPass,
                                0,
                                pipelineLayout,
                                shaderStageCreateInfos,
                                &vertexInputStateCreateInfo,
                                &inputAssemblyStateCreateInfo,
                                &viewportCreateInfo,
                                &rasterizationStateCreateInfo,
                                &multisampleStateCreateInfo,
                                nullptr,
                                &colorBlendStateCreateInfo,
                                nullptr);
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
        commandBuffer.draw(3,
                           1,
                           0,
                           0);
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