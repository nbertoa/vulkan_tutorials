#include "App.h"

#include "utils/PipelineStateFactory.h"

using namespace vk;

App::App(const uint32_t windowWidth,
         const uint32_t windowHeight,
         const char* windowTitle)
    : mSystemManager(windowWidth, windowHeight, windowTitle)
    , mRenderPass(createRenderPass())
{
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = PipelineStateFactory::emptyVertexInputState();
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = 
        PipelineStateFactory::createInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                       VK_FALSE);
    VkPipelineViewportStateCreateInfo viewportCreateInfo = mSystemManager.swapChain().pipelineViewportCreateInfo();
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = PipelineStateFactory::defaultRasterizationState();
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = PipelineStateFactory::disableMultisampleState();
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo =
        PipelineStateFactory::colorBlendState(PipelineStateFactory::disableColorBlendAttachmentState());

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
    mGraphicsPipeline.reset(new GraphicsPipeline(mSystemManager.logicalDevice(),
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
                                                 nullptr));

    mFrameBuffers.reset(new FrameBuffers(mSystemManager.logicalDevice(),
                                         mSystemManager.swapChain(),
                                         *mRenderPass));

    mCommandBuffers.reset(new CommandBuffers(mSystemManager.logicalDevice(),
                                             mSystemManager.graphicsCommandPool(),
                                             mFrameBuffers->bufferCount(),
                                             VK_COMMAND_BUFFER_LEVEL_PRIMARY));

    for (size_t i = 0; i < mCommandBuffers->bufferCount(); ++i) {
        mCommandBuffers->beginRecording(i,
                                        VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

        mCommandBuffers->beginPass(i,
                                   *mRenderPass,
                                   mFrameBuffers->buffer(i),
                                   mSystemManager.swapChain().imageExtent());
        mCommandBuffers->bindPipeline(i,
                                      *mGraphicsPipeline);
        mCommandBuffers->draw(i,
                              3,
                              1,
                              0,
                              0);
        mCommandBuffers->endPass(i);
        mCommandBuffers->endRecording(i);
    }
}

void 
App::run() {
    while (glfwWindowShouldClose(&mSystemManager.window().glfwWindow()) == 0) {
        glfwPollEvents();
    }
}

RenderPass*
App::createRenderPass() const {
    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.format = mSystemManager.swapChain().imageFormat();
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachmentReference = {};
    attachmentReference.attachment = 0;
    attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &attachmentReference;

    return new RenderPass(mSystemManager.logicalDevice(),
                          {attachmentDescription},
                          {subpassDescription});
}