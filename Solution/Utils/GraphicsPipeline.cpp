#include "GraphicsPipeline.h"

#include <cassert>

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
                                   const VkPipelineDynamicStateCreateInfo* dynamicState,
                                   const VkPipelineTessellationStateCreateInfo* tessellationState)
    : mLogicalDevice(logicalDevice)
    , mPipelineLayout(std::move(pipelineLayout))
{
    assert(mPipelineLayout.get() != nullptr);

    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    createInfo.pStages = shaderStages.empty() ? nullptr : shaderStages.data();
    createInfo.pVertexInputState = vertexInputState;
    createInfo.pInputAssemblyState = inputAssemblyState;
    createInfo.pViewportState = viewportState;
    createInfo.pRasterizationState = rasterizationState;
    createInfo.pMultisampleState = multisampleState;
    createInfo.pDepthStencilState = depthStencilState;
    createInfo.pColorBlendState = colorBlendState;
    createInfo.pDynamicState = dynamicState;
    createInfo.pTessellationState = tessellationState;
    createInfo.layout = mPipelineLayout->pipelineLayout();
    createInfo.renderPass = renderPass.vkRenderPass();
    createInfo.subpass = subPassIndex;

    // Vulkan allows you to create a new graphics pipeline by
    // deriving from an existing pipeline. The idea of pipeline
    // derivatives is that it is less expensive to set up pipelines
    // when they have much functionality in common with an existing
    // pipeline and switching between pipelines from the same
    // parent can also be done quicker.
    // We disable this.
    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = -1;

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

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mPipelineLayout(std::move(other.mPipelineLayout))
    , mPipeline(other.mPipeline)
{
    other.mPipeline = VK_NULL_HANDLE;
}

VkPipeline 
GraphicsPipeline::vkPipeline() const {
    assert(mPipeline != VK_NULL_HANDLE);
    return mPipeline;
}

}