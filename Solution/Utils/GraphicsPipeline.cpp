#include "GraphicsPipeline.h"

#include <cassert>

#include "DebugUtils.h"
#include "device/LogicalDevice.h"
#include "pipeline_stage/PipelineStates.h"
#include "shader/ShaderStages.h"

namespace vk2 {
GraphicsPipeline::GraphicsPipeline(vk::UniquePipelineLayout& pipelineLayout,
                                   const PipelineStates& pipelineStates,
                                   const ShaderStages& shaderStages,
                                   const vk::RenderPass& renderPass,
                                   const uint32_t subPassIndex)
    : mPipelineLayout(std::move(pipelineLayout)) {
    vk::GraphicsPipelineCreateInfo createInfo
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

    mPipeline = LogicalDevice::device().createGraphicsPipelineUnique(vk::PipelineCache(),
                                                                     createInfo);
}

vk::Pipeline 
GraphicsPipeline::pipeline() const {
    assert(mPipeline.get() != VK_NULL_HANDLE);
    return mPipeline.get();
}

vk::PipelineLayout
GraphicsPipeline::pipelineLayout() const {
    assert(mPipelineLayout.get() != VK_NULL_HANDLE);
    return mPipelineLayout.get();
}

}