#include "GraphicsPipeline.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"
#include "../pipeline_stage/PipelineStates.h"
#include "../shader/ShaderStages.h"

namespace vk2 {
GraphicsPipeline::GraphicsPipeline(vk::UniquePipelineLayout& pipelineLayout,
                                   const PipelineStates& pipelineStates,
                                   const ShaderStages& shaderStages,
                                   const vk::RenderPass& renderPass,
                                   const uint32_t subPassIndex)
    : mPipelineLayout(std::move(pipelineLayout)) {
    vk::GraphicsPipelineCreateInfo createInfo =
    {
        vk::PipelineCreateFlags(),
        static_cast<uint32_t>(shaderStages.stages().size()),
        shaderStages.stages().empty() ? nullptr : shaderStages.stages().data(),
        pipelineStates.vertexInputState() != nullptr ?
            &pipelineStates.vertexInputState()->state() :
            nullptr,
        pipelineStates.inputAssemblyState() != nullptr ?
            &pipelineStates.inputAssemblyState()->state() :
            nullptr,
        pipelineStates.tessellationState() != nullptr ?
            &pipelineStates.tessellationState()->state() :
            nullptr,
        pipelineStates.viewportState() != nullptr ?
            &pipelineStates.viewportState()->state() :
            nullptr,
        pipelineStates.rasterizationState() != nullptr ?
            &pipelineStates.rasterizationState()->state() :
            nullptr,
        pipelineStates.multisampleState() != nullptr ?
            &pipelineStates.multisampleState()->state() :
            nullptr,
        pipelineStates.depthStencilState() != nullptr ?
            &pipelineStates.depthStencilState()->state() :
            nullptr,
        pipelineStates.colorBlendState() != nullptr ?
            &pipelineStates.colorBlendState()->state() :
            nullptr,
        pipelineStates.dynamicState() != nullptr ?
            &pipelineStates.dynamicState()->state() :
            nullptr,
        mPipelineLayout.get(),
        renderPass,
        subPassIndex,
        vk::Pipeline(), // base pipeline handle
        -1 // base pipeline index
    };

    vkChecker(vkCreateGraphicsPipelines(LogicalDevice::device(),
                                        VK_NULL_HANDLE,
                                        1,
                                        (VkGraphicsPipelineCreateInfo*)&createInfo,
                                        nullptr,
                                        &mPipeline));
    assert(mPipeline != VK_NULL_HANDLE);
}

GraphicsPipeline::~GraphicsPipeline() {
    assert(mPipeline != VK_NULL_HANDLE);
    vkDestroyPipeline(LogicalDevice::device(),
                      mPipeline,
                      nullptr);
}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
    : mPipeline(other.mPipeline)
    , mPipelineLayout(std::move(other.mPipelineLayout))
{
    other.mPipeline = VK_NULL_HANDLE;
}

VkPipeline 
GraphicsPipeline::vkPipeline() const {
    assert(mPipeline != VK_NULL_HANDLE);
    return mPipeline;
}

vk::PipelineLayout
GraphicsPipeline::pipelineLayout() const {
    assert(mPipeline != VK_NULL_HANDLE);
    return mPipelineLayout.get();
}

}