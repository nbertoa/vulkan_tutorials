#include "GraphicsPipelineCreator.h"

#include "Utils/DescriptorSetLayout.h"
#include "Utils/GraphicsPipeline.h"
#include "Utils/LogicalDevice.h"
#include "Utils/PipelineStateFactory.h"
#include "Utils/RenderPass.h"
#include "Utils///ShaderModule.h"
#include "Utils/SwapChain.h"
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
                                                 "../../QuadWithUniformBuffer/resources/shaders/vert.spv",
                                                 VK_SHADER_STAGE_VERTEX_BIT);

           const ShaderModule fragmentShaderModule(logicalDevice,
                                                   "../../QuadWithUniformBuffer/resources/shaders/frag.spv",
                                                   VK_SHADER_STAGE_FRAGMENT_BIT);

           std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos =
           {
               vertexShaderModule.pipelineShaderStageCreateInfo(),
               fragmentShaderModule.pipelineShaderStageCreateInfo(),
           };

           std::unique_ptr<PipelineLayout> pipelineLayout(createPipelineLayout(logicalDevice));
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