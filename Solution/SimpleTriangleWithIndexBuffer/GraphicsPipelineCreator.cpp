#include "GraphicsPipelineCreator.h"

#include "Utils/GraphicsPipeline.h"
#include "Utils/LogicalDevice.h"
#include "Utils/PipelineStateFactory.h"
#include "Utils/RenderPass.h"
#include "Utils///ShaderModule.h"
#include "Utils/SwapChain.h"
#include "Utils/vertex/PosColorVertex.h"

using namespace vk;

GraphicsPipelineCreator graphicsPipelineCreator() {
    return 
    [](const LogicalDevice& logicalDevice,
       const SwapChain& swapChain,
       const RenderPass& renderPass) {
           std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
           PosColorVertex::vertexInputBindingDescriptions(vertexInputBindingDescriptions);

           std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription;
           PosColorVertex::vertexInputAttributeDescriptions(vertexInputAttributeDescription);

           VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
           PipelineStateFactory::vertexInputState(vertexInputBindingDescriptions,
                                                  vertexInputAttributeDescription,
                                                  vertexInputStateCreateInfo);

           VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
           PipelineStateFactory::createInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                          VK_FALSE,
                                                          inputAssemblyStateCreateInfo);

           VkPipelineViewportStateCreateInfo viewportCreateInfo = swapChain.pipelineViewportCreateInfo();

           VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
           PipelineStateFactory::defaultRasterizationState(rasterizationStateCreateInfo);

           VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
           PipelineStateFactory::disableMultisampleState(multisampleStateCreateInfo);

           VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
           PipelineStateFactory::enableColorBlendAttachmentState(colorBlendAttachmentState);

           VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
           PipelineStateFactory::colorBlendState(colorBlendAttachmentState, colorBlendStateCreateInfo);

           const ShaderModule vertexShaderModule(logicalDevice,
                                                 "../../SimpleTriangleWithIndexBuffer/resources/shaders/vert.spv",
                                                 VK_SHADER_STAGE_VERTEX_BIT);

           const ShaderModule fragmentShaderModule(logicalDevice,
                                                   "../../SimpleTriangleWithIndexBuffer/resources/shaders/frag.spv",
                                                   VK_SHADER_STAGE_FRAGMENT_BIT);

           std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos =
           {
               vertexShaderModule.pipelineShaderStageCreateInfo(),
               fragmentShaderModule.pipelineShaderStageCreateInfo(),
           };

           std::unique_ptr<PipelineLayout> pipelineLayout(new PipelineLayout(logicalDevice));
           return new GraphicsPipeline(logicalDevice,
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
    };
}