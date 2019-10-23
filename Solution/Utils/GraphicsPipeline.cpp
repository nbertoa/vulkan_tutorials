#include "GraphicsPipeline.h"

#include <cassert>

#include "DebugUtils.h"
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "pipeline_stage/ColorBlendState.h"
#include "pipeline_stage/DepthStencilState.h"
#include "pipeline_stage/DynamicState.h"
#include "pipeline_stage/InputAssemblyState.h"
#include "pipeline_stage/MultisampleState.h"
#include "pipeline_stage/RasterizationState.h"
#include "pipeline_stage/ShaderStages.h"
#include "pipeline_stage/TessellationState.h"
#include "pipeline_stage/VertexInputState.h"
#include "pipeline_stage/ViewportState.h"

namespace vk {
GraphicsPipeline::GraphicsPipeline(const LogicalDevice& logicalDevice,
                                   const RenderPass& renderPass,
                                   const uint32_t subPassIndex,
                                   std::unique_ptr<PipelineLayout>& pipelineLayout,
                                   const ColorBlendState* colorBlendState,
                                   const DepthStencilState* depthStencilState,
                                   const DynamicState* dynamicState,
                                   const InputAssemblyState* inputAssemblyState,
                                   const MultisampleState* multisampleState,
                                   const RasterizationState* rasterizationState,
                                   const ShaderStages& shaderStages,
                                   const TessellationState* tessellationState,
                                   const VertexInputState* vertexInputState,
                                   const ViewportState* viewportState)
    : mLogicalDevice(logicalDevice)
    , mPipelineLayout(std::move(pipelineLayout))
{
    assert(mPipelineLayout.get() != nullptr);

    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = static_cast<uint32_t>(shaderStages.vkStages().size());
    createInfo.pStages = shaderStages.vkStages().empty() ? nullptr : shaderStages.vkStages().data();
    createInfo.pVertexInputState = vertexInputState != nullptr ? &vertexInputState->vkState() : nullptr;
    createInfo.pInputAssemblyState = inputAssemblyState != nullptr ? &inputAssemblyState->vkState() : nullptr;
    createInfo.pViewportState = viewportState != nullptr ? &viewportState->vkState() : nullptr;
    createInfo.pRasterizationState = rasterizationState != nullptr ? &rasterizationState->vkState() : nullptr;
    createInfo.pMultisampleState = multisampleState != nullptr ? &multisampleState->vkState() : nullptr;
    createInfo.pDepthStencilState = depthStencilState != nullptr ? &depthStencilState->vkState() : nullptr;
    createInfo.pColorBlendState = colorBlendState != nullptr ? &colorBlendState->vkState() : nullptr;
    createInfo.pDynamicState = dynamicState != nullptr ? &dynamicState->vkState() : nullptr;
    createInfo.pTessellationState = tessellationState != nullptr ? &tessellationState->vkState() : nullptr;
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