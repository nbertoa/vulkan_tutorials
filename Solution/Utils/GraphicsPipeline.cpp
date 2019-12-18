#include "GraphicsPipeline.h"

#include <cassert>

<<<<<<< HEAD:Solution/Utils/GraphicsPipeline.cpp
#include "DebugUtils.h"
#include "device/LogicalDevice.h"
#include "pipeline_stage/PipelineStates.h"
#include "shader/ShaderStages.h"
=======
#include "../device/LogicalDevice.h"
#include "../pipeline/PipelineStates.h"
#include "../shader/ShaderStages.h"
>>>>>>> 677cea1a73754f3e5e00c5d3f9cda5f558daf293:Solution/Utils/pipeline/GraphicsPipeline.cpp

namespace vulkan {
GraphicsPipeline::GraphicsPipeline(vk::UniquePipelineLayout& pipelineLayout,
                                   const PipelineStates& pipelineStates,
                                   const ShaderStages& shaderStages,
                                   const vk::RenderPass renderPass,
                                   const uint32_t subPassIndex)
    : mPipelineLayout(std::move(pipelineLayout)) {
    vk::GraphicsPipelineCreateInfo info;
    info.setStageCount(static_cast<uint32_t>(shaderStages.stages().size()));
    info.setPStages(shaderStages.stages().empty() ? nullptr : shaderStages.stages().data());
    info.setPVertexInputState(pipelineStates.vertexInputState() != nullptr ?
                              &pipelineStates.vertexInputState()->state() :
                              nullptr);
    info.setPInputAssemblyState(pipelineStates.inputAssemblyState() != nullptr ?
                                &pipelineStates.inputAssemblyState()->state() :
                                nullptr);
    info.setPTessellationState(pipelineStates.tessellationState() != nullptr ?
                               &pipelineStates.tessellationState()->state() :
                               nullptr);
    info.setPViewportState(pipelineStates.viewportState() != nullptr ?
                           &pipelineStates.viewportState()->state() :
                           nullptr);
    info.setPRasterizationState(pipelineStates.rasterizationState() != nullptr ?
                                &pipelineStates.rasterizationState()->state() :
                                nullptr);
    info.setPMultisampleState(pipelineStates.multisampleState() != nullptr ?
                              &pipelineStates.multisampleState()->state() :
                              nullptr);
    info.setPDepthStencilState(pipelineStates.depthStencilState() != nullptr ?
                               &pipelineStates.depthStencilState()->state() :
                               nullptr);
    info.setPColorBlendState(pipelineStates.colorBlendState() != nullptr ?
                             &pipelineStates.colorBlendState()->state() :
                             nullptr);
    info.setPDynamicState(pipelineStates.dynamicState() != nullptr ?
                          &pipelineStates.dynamicState()->state() :
                          nullptr);
    info.setLayout(mPipelineLayout.get());
    info.setRenderPass(renderPass);
    info.setSubpass(subPassIndex);

    mPipeline = LogicalDevice::device().createGraphicsPipelineUnique(vk::PipelineCache(),
                                                                     info);
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