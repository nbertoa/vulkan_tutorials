#include "utils/AppInstance.h"
#include "utils/GraphicsPipeline.h"
#include "utils/LogicalDevice.h"
#include "utils/PipelineStateFactory.h"
#include "utils/RenderPass.h"
#include "utils/ShaderModule.h"
#include "utils/SwapChain.h"
#include "utils/Window.h"
#include "utils/WindowSurface.h"

using namespace vk;

int main() {
    const uint32_t windowWidth = 800;
    const uint32_t windowHeight = 600;

    Window window(windowWidth, 
                  windowHeight, 
                  "Vulkan window");

    AppInstance instance;

    WindowSurface windowSurface(instance.vkInstance(), 
                                window.glfwWindow());

    LogicalDevice logicalDevice(instance.vkInstance(), 
                                windowSurface);

    SwapChain swapChain(windowWidth, 
                        windowHeight, 
                        windowSurface, 
                        logicalDevice);

    ShaderModule vertexShaderModule(logicalDevice,
                                    "../../SimpleTriangle/resources/shaders/vert.spv",
                                    VK_SHADER_STAGE_VERTEX_BIT);

    ShaderModule fragShaderModule(logicalDevice,
                                  "../../SimpleTriangle/resources/shaders/frag.spv",
                                  VK_SHADER_STAGE_FRAGMENT_BIT);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos 
    {
        vertexShaderModule.pipelineShaderStageCreateInfo(),
        fragShaderModule.pipelineShaderStageCreateInfo(),
    };

    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.format = swapChain.imageFormat();
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

    RenderPass renderPass(logicalDevice,
                          {attachmentDescription},
                          {subpassDescription});

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = PipelineStateFactory::emptyVertexInputState();
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = PipelineStateFactory::createInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                                                                                         VK_FALSE);
    VkPipelineViewportStateCreateInfo viewportCreateInfo = swapChain.pipelineViewportCreateInfo();
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = PipelineStateFactory::defaultRasterizationState();
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = PipelineStateFactory::disableMultisampleState();
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = 
        PipelineStateFactory::colorBlendState(PipelineStateFactory::disableColorBlendAttachmentState());
                          
    std::unique_ptr<PipelineLayout> pipelineLayout(new PipelineLayout(logicalDevice));
    GraphicsPipeline graphicsPipeline(logicalDevice,
                                      renderPass,
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

    while (glfwWindowShouldClose(&window.glfwWindow()) == 0) {
        glfwPollEvents();
    }
}