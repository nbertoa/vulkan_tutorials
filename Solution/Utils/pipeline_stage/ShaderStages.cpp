#include "ShaderStages.h"

#include "../ShaderModule.h"

namespace vk {
void
ShaderStages::addShaderModule(const ShaderModule& shaderModule) {
    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage = shaderModule.shaderStageFlag();
    createInfo.module = shaderModule.vkShaderModule();
    createInfo.pName = shaderModule.entryPointName();

    mCreateInfoVec.push_back(createInfo);
}

const std::vector<VkPipelineShaderStageCreateInfo>&
ShaderStages::vkStages() const {
    return mCreateInfoVec;
}
}