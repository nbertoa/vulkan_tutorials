#include "ShaderStages.h"

#include "ShaderModule.h"

namespace vulkan {
void
ShaderStages::addShaderModule(const ShaderModule& shaderModule) {
    vk::PipelineShaderStageCreateInfo createInfo = 
    {
        vk::PipelineShaderStageCreateFlags(),
        shaderModule.shaderStageFlag(),
        shaderModule.module(),
        shaderModule.entryPointName()
    };

    mCreateInfoVec.emplace_back(createInfo);
}

const std::vector<vk::PipelineShaderStageCreateInfo>&
ShaderStages::stages() const {
    return mCreateInfoVec;
}
}