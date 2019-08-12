#include "ShaderModule.h"

#include <cassert>
#include <fstream> 

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
ShaderModule::ShaderModule(const char* shaderByteCodePath, const LogicalDevice& logicalDevice)
    : mLogicalDevice(logicalDevice)
    , mShaderByteCodePath(shaderByteCodePath)
{
    assert(mShaderByteCodePath != nullptr);
    createShaderModule();
}

ShaderModule::~ShaderModule() {
    assert(mShaderByteCodePath != nullptr);
    assert(mShaderModule != VK_NULL_HANDLE);

    vkDestroyShaderModule(mLogicalDevice.vkDevice(), 
                          mShaderModule, 
                          nullptr);
}

std::vector<char>
ShaderModule::readFile(const char* filename) {
    assert(filename != nullptr);

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
    assert(mShaderByteCodePath != nullptr);
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