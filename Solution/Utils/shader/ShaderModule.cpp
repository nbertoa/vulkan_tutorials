#include "ShaderModule.h"

#include <cassert>
#include <fstream> 

#include "../device/LogicalDevice.h"

namespace vulkan {
ShaderModule::ShaderModule(const std::string& shaderByteCodePath,
                           const vk::ShaderStageFlagBits shaderStageFlag,
                           const char* entryPointName)
    : mShaderStageFlag(shaderStageFlag)
    , mShaderByteCodePath(shaderByteCodePath)
    , mEntryPointName(entryPointName)
{
    assert(shaderByteCodePath.empty() == false);
    assert(entryPointName != nullptr);

    const std::vector<char> shaderByteCode = readFile(mShaderByteCodePath);

    vk::ShaderModuleCreateInfo info;
    info.setCodeSize(shaderByteCode.size());
    info.setPCode(reinterpret_cast<const uint32_t*>(shaderByteCode.data()));
    mShaderModule = LogicalDevice::device().createShaderModuleUnique(info);
}

const std::string& 
ShaderModule::shaderByteCodePath() const {
    assert(mShaderModule.get() != VK_NULL_HANDLE);
    return mShaderByteCodePath;
}

vk::ShaderStageFlagBits 
ShaderModule::shaderStageFlag() const {
    assert(mShaderModule.get() != VK_NULL_HANDLE);
    return mShaderStageFlag;
}

vk::ShaderModule
ShaderModule::module() const {
    assert(mShaderModule.get() != VK_NULL_HANDLE);
    return mShaderModule.get();
}

const char*
ShaderModule::entryPointName() const {
    assert(mShaderModule.get() != VK_NULL_HANDLE);
    return mEntryPointName;
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
}