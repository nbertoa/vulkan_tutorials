#include "GraphicsPipelineCreator.h"

#include "Utils/DescriptorSetLayout.h"
#include "Utils/GraphicsPipeline.h"
#include "Utils/LogicalDevice.h"
#include "Utils/RenderPass.h"
#include "Utils/ShaderModule.h"
#include "Utils/SwapChain.h"
#include "Utils/pipeline_stage/ColorBlendAttachmentState.h"
#include "Utils/pipeline_stage/ColorBlendState.h"
#include "Utils/pipeline_stage/DepthStencilState.h"
#include "Utils/pipeline_stage/DynamicState.h"
#include "Utils/pipeline_stage/InputAssemblyState.h"
#include "Utils/pipeline_stage/MultisampleState.h"
#include "Utils/pipeline_stage/RasterizationState.h"
#include "Utils/pipeline_stage/ShaderStages.h"
#include "Utils/pipeline_stage/TessellationState.h"
#include "Utils/pipeline_stage/VertexInputState.h"
#include "Utils/pipeline_stage/ViewportState.h"
#include "Utils/vertex/PosColorVertex.h"

using namespace vk;

namespace {
PipelineLayout* createPipelineLayout(const LogicalDevice& logicalDevice) {
    VkDescriptorSetLayoutBinding uboDescriptorSetLayoutBinding = {};
    uboDescriptorSetLayoutBinding.binding = 0;
    uboDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboDescriptorSetLayoutBinding.descriptorCount = 1;

    std::vector<VkDescriptorSetLayoutBinding> uboDescriptorSetLayoutBindings;
    uboDescriptorSetLayoutBindings.push_back(uboDescriptorSetLayoutBinding);

    DescriptorSetLayout descriptorSetLayout(logicalDevice,
                                            uboDescriptorSetLayoutBindings);

    return new PipelineLayout(logicalDevice,
                              &descriptorSetLayout,
                              nullptr);
}
}

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

           const ViewportState viewportState(swapChain.viewport(),
                                             swapChain.scissorRect());

           const RasterizationState rasterizationState;

           const MultisampleState multisampleState;

           const ColorBlendAttachmentState colorBlendAttachmentState;
           const ColorBlendState colorBlendState(colorBlendAttachmentState);

           const ShaderModule vertexShaderModule(logicalDevice,
                                                 "../../QuadWithUniformBuffer/resources/shaders/vert.spv",
                                                 VK_SHADER_STAGE_VERTEX_BIT);

           const ShaderModule fragmentShaderModule(logicalDevice,
                                                   "../../QuadWithUniformBuffer/resources/shaders/frag.spv",
                                                   VK_SHADER_STAGE_FRAGMENT_BIT);

           ShaderStages shaderStages;
           shaderStages.addShaderModule(vertexShaderModule);
           shaderStages.addShaderModule(fragmentShaderModule);

           std::unique_ptr<PipelineLayout> pipelineLayout(createPipelineLayout(logicalDevice));
           return new GraphicsPipeline(logicalDevice,
                                       renderPass,
                                       0,
                                       pipelineLayout,
                                       &colorBlendState,
                                       nullptr, // depth stencil state
                                       nullptr, // dynamic state
                                       &inputAssemblyState,
                                       &multisampleState,
                                       &rasterizationState,
                                       shaderStages,
                                       nullptr, // tessellation state
                                       &vertexInputState,
                                       &viewportState);
    };
}