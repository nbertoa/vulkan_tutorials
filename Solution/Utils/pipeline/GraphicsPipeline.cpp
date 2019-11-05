#include "GraphicsPipeline.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"
#include "../pipeline_stage/PipelineStates.h"
#include "../render_pass/RenderPass.h"
#include "../shader/ShaderStages.h"

namespace vk {
GraphicsPipeline::GraphicsPipeline(const LogicalDevice& logicalDevice,
                                   const RenderPass& renderPass,
                                   const uint32_t subPassIndex,
                                   PipelineLayout& pipelineLayout,
                                   const PipelineStates& pipelineStates,
                                   const ShaderStages& shaderStages)
    : mLogicalDevice(logicalDevice)
    , mPipelineLayout(std::move(pipelineLayout))
{
    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = static_cast<uint32_t>(shaderStages.vkStages().size());
    createInfo.pStages = shaderStages.vkStages().empty() ? nullptr : shaderStages.vkStages().data();
    createInfo.pVertexInputState = 
        pipelineStates.vertexInputState() != nullptr ? &pipelineStates.vertexInputState()->vkState() : nullptr;
    createInfo.pInputAssemblyState = 
        pipelineStates.inputAssemblyState() != nullptr ? &pipelineStates.inputAssemblyState()->vkState() : nullptr;
    createInfo.pViewportState = 
        pipelineStates.viewportState() != nullptr ? &pipelineStates.viewportState()->vkState() : nullptr;
    createInfo.pRasterizationState = 
        pipelineStates.rasterizationState() != nullptr ? &pipelineStates.rasterizationState()->vkState() : nullptr;
    createInfo.pMultisampleState = 
        pipelineStates.multisampleState() != nullptr ? &pipelineStates.multisampleState()->vkState() : nullptr;
    createInfo.pDepthStencilState = 
        pipelineStates.depthStencilState() != nullptr ? &pipelineStates.depthStencilState()->vkState() : nullptr;
    createInfo.pColorBlendState = 
        pipelineStates.colorBlendState() != nullptr ? &pipelineStates.colorBlendState()->vkState() : nullptr;
    createInfo.pDynamicState = 
        pipelineStates.dynamicState() != nullptr ? &pipelineStates.dynamicState()->vkState() : nullptr;
    createInfo.pTessellationState = 
        pipelineStates.tessellationState() != nullptr ? &pipelineStates.tessellationState()->vkState() : nullptr;
    createInfo.layout = mPipelineLayout.vkPipelineLayout();
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
    , mPipeline(other.mPipeline)
    , mPipelineLayout(std::move(other.mPipelineLayout))
{
    other.mPipeline = VK_NULL_HANDLE;
}

VkPipeline 
GraphicsPipeline::vkPipeline() const {
    assert(mPipeline != VK_NULL_HANDLE);
    return mPipeline;
}

const PipelineLayout&
GraphicsPipeline::pipelineLayout() const {
    assert(mPipeline != VK_NULL_HANDLE);
    return mPipelineLayout;
}

}