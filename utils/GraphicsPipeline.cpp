#include "GraphicsPipeline.h"

#include "DebugUtils.h"
#include "LogicalDevice.h"
#include "RenderPass.h"

namespace vk {
GraphicsPipeline::GraphicsPipeline(const LogicalDevice& logicalDevice,
                                   const RenderPass& renderPass,
                                   const uint32_t subPassIndex,
                                   std::unique_ptr<PipelineLayout>& pipelineLayout,
                                   const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
                                   const VkPipelineVertexInputStateCreateInfo* vertexInputState,
                                   const VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState,
                                   const VkPipelineViewportStateCreateInfo* viewportState,
                                   const VkPipelineRasterizationStateCreateInfo* rasterizationState,
                                   const VkPipelineMultisampleStateCreateInfo* multisampleState,
                                   const VkPipelineDepthStencilStateCreateInfo* depthStencilState,
                                   const VkPipelineColorBlendStateCreateInfo* colorBlendState,
                                   const VkPipelineDynamicStateCreateInfo* dynamicState)
    : mLogicalDevice(logicalDevice)
    , mPipelineLayout(std::move(pipelineLayout))
{
    assert(mPipelineLayout.get() != nullptr);

    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    // Shader stages
    createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    createInfo.pStages = shaderStages.empty() ? nullptr : shaderStages.data();

    // Fixed-function state
    createInfo.pVertexInputState = vertexInputState;
    createInfo.pInputAssemblyState = inputAssemblyState;
    createInfo.pViewportState = viewportState;
    createInfo.pRasterizationState = rasterizationState;
    createInfo.pMultisampleState = multisampleState;
    createInfo.pDepthStencilState = depthStencilState;
    createInfo.pColorBlendState = colorBlendState;
    createInfo.pDynamicState = dynamicState;

    // Layout
    createInfo.layout = mPipelineLayout->pipelineLayout();

    // Render pass
    createInfo.renderPass = renderPass.vkRenderPass();
    createInfo.subpass = subPassIndex;

    vkChecker(vkCreateGraphicsPipelines(mLogicalDevice.vkDevice(),
                                        VK_NULL_HANDLE,
                                        1,
                                        &createInfo,
                                        nullptr,
                                        &mPipeline));
    assert(mPipeline != VK_NULL_HANDLE);
}

GraphicsPipeline::~GraphicsPipeline() {
    assert(mPipeline != VK_NULL_HANDLE);
    vkDestroyPipeline(mLogicalDevice.vkDevice(),
                      mPipeline,
                      nullptr);
}

}