#include "App.h"

using namespace vk;

App::App(const uint32_t windowWidth,
         const uint32_t windowHeight,
         const char* windowTitle)
    : mWindow(new Window(windowWidth, windowHeight, windowTitle))
    , mAppInstance(new AppInstance())
    , mWindowSurface(new WindowSurface(*mAppInstance, *mWindow))
    , mLogicalDevice(new LogicalDevice(*mAppInstance, *mWindowSurface))
    , mSwapChain(new SwapChain(*mLogicalDevice, *mWindow, *mWindowSurface))
    , mRenderPass(createRenderPass())
{
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = PipelineStateFactory::emptyVertexInputState();
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = 
        PipelineStateFactory::createInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                       VK_FALSE);
    VkPipelineViewportStateCreateInfo viewportCreateInfo = mSwapChain->pipelineViewportCreateInfo();
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = PipelineStateFactory::defaultRasterizationState();
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = PipelineStateFactory::disableMultisampleState();
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo =
        PipelineStateFactory::colorBlendState(PipelineStateFactory::disableColorBlendAttachmentState());

    const ShaderModule vertexShaderModule(*mLogicalDevice,
                                    "../../SimpleTriangle/resources/shaders/vert.spv",
                                    VK_SHADER_STAGE_VERTEX_BIT);

    const ShaderModule fragmentShaderModule(*mLogicalDevice,
                                            "../../SimpleTriangle/resources/shaders/frag.spv",
                                            VK_SHADER_STAGE_FRAGMENT_BIT);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos = 
    {
        vertexShaderModule.pipelineShaderStageCreateInfo(),
        fragmentShaderModule.pipelineShaderStageCreateInfo(),
    };

    std::unique_ptr<PipelineLayout> pipelineLayout(new PipelineLayout(*mLogicalDevice));
    mGraphicsPipeline.reset(new GraphicsPipeline(*mLogicalDevice,
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

    mFrameBuffers.reset(new FrameBuffers(*mLogicalDevice,
                                         *mSwapChain,
                                         *mRenderPass));
}

void 
App::run() {
    assert(mWindow != nullptr);

    while (glfwWindowShouldClose(&mWindow->glfwWindow()) == 0) {
        glfwPollEvents();
    }
}

RenderPass*
App::createRenderPass() const {
    assert(mSwapChain != nullptr);
    assert(mLogicalDevice != nullptr);

    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.format = mSwapChain->imageFormat();
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

    return new RenderPass(*mLogicalDevice,
                          {attachmentDescription},
                          {subpassDescription});
}