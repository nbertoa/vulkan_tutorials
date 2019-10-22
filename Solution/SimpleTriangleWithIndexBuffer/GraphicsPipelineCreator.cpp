#include "GraphicsPipelineCreator.h"

#include "Utils/GraphicsPipeline.h"
#include "Utils/LogicalDevice.h"
#include "Utils/PipelineStateFactory.h"
#include "Utils/RenderPass.h"
#include "Utils///ShaderModule.h"
#include "Utils/SwapChain.h"
#include "Utils/pipeline_stage/ColorBlendAttachmentState.h"
#include "Utils/vertex/PosColorVertex.h"

using namespace vk;

GraphicsPipelineCreator graphicsPipelineCreator() {
    return 
    [](const LogicalDevice& logicalDevice,
       const SwapChain& swapChain,
       const RenderPass& renderPass) {
           std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
           PosColorVertex::vertexInputBindingDescriptions(vertexInputBindingDescriptions);

           std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
           PosColorVertex::vertexInputAttributeDescriptions(vertexInputAttributeDescriptions);

           const VertexInputState vertexInputState(vertexInputBindingDescriptions,
                                                   vertexInputAttributeDescriptions);

           InputAssemblyState inputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                 VK_FALSE);

           VkPipelineViewportStateCreateInfo viewportCreateInfo = swapChain.viewportState();

           const RasterizationState rasterizationState;

           const MultisampleState multisampleState;

           const ColorBlendAttachmentState colorBlendAttachmentState;
           const ColorBlendState colorBlendState(colorBlendAttachmentState);

           const ShaderModule vertexShaderModule(logicalDevice,
                                                 "../../SimpleTriangleWithIndexBuffer/resources/shaders/vert.spv",
                                                 VK_SHADER_STAGE_VERTEX_BIT);

           const ShaderModule fragmentShaderModule(logicalDevice,
                                                   "../../SimpleTriangleWithIndexBuffer/resources/shaders/frag.spv",
                                                   VK_SHADER_STAGE_FRAGMENT_BIT);

           std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos =
           {
               vertexShaderModule.shaderStage(),
               fragmentShaderModule.shaderStage(),
           };

           std::unique_ptr<PipelineLayout> pipelineLayout(new PipelineLayout(logicalDevice));
           return new GraphicsPipeline(logicalDevice,
                                       renderPass,
                                       0,
                                       pipelineLayout,
                                       shaderStageCreateInfos,
                                       &colorBlendState,
                                       &inputAssemblyState,
                                       &multisampleState,
                                       &rasterizationState,
                                       &vertexInputState,                                       
                                       &viewportCreateInfo,                                                              
                                       nullptr,                                       
                                       nullptr,
                                       nullptr);
    };
}