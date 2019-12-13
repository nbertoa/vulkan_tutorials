#include "ShaderStages.h"

#include "ShaderModule.h"

namespace vulkan {
void
ShaderStages::addShaderModule(const ShaderModule& shaderModule) {
    vk::PipelineShaderStageCreateInfo info;
    info.setModule(shaderModule.module());
    info.setPName(shaderModule.entryPointName());
    info.setStage(shaderModule.shaderStageFlag());
    mCreateInfoVec.emplace_back(info);
}

const std::vector<vk::PipelineShaderStageCreateInfo>&
ShaderStages::stages() const {
    return mCreateInfoVec;
}
}