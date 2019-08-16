#include "ShaderModule.h"

#include <fstream> 

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
ShaderModule::ShaderModule(const LogicalDevice& logicalDevice,
                           const std::string& shaderByteCodePath,
                           const VkShaderStageFlagBits shaderStageFlag)
    : mLogicalDevice(logicalDevice)
    , mShaderStageFlag(shaderStageFlag)
    , mShaderByteCodePath(shaderByteCodePath)
{
    createShaderModule();
}

ShaderModule::~ShaderModule() {
    assert(mShaderModule != VK_NULL_HANDLE);

    vkDestroyShaderModule(mLogicalDevice.vkDevice(), 
                          mShaderModule, 
                          nullptr);
}

VkPipelineShaderStageCreateInfo 
ShaderModule::pipelineShaderStageCreateInfo() const {
    assert(mShaderModule != VK_NULL_HANDLE);

    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage = mShaderStageFlag;
    createInfo.module = mShaderModule;
    createInfo.pName = "main";

    return createInfo;
}

std::vector<char>
ShaderModule::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    assert(file.is_open());

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

void
ShaderModule::createShaderModule() {
    assert(mShaderModule == VK_NULL_HANDLE);

    const std::vector<char> shaderByteCode = readFile(mShaderByteCodePath);

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderByteCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderByteCode.data());

    vkChecker(vkCreateShaderModule(mLogicalDevice.vkDevice(), 
                                   &createInfo, 
                                   nullptr, 
                                   &mShaderModule));
    assert(mShaderModule != VK_NULL_HANDLE);
}
}