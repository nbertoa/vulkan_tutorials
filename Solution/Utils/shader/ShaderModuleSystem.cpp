#include "ShaderModuleSystem.h"

#include <cassert>
#include <cstring>

namespace vk {
ShaderModuleSystem::ShaderModuleSystem(const LogicalDevice& logicalDevice) 
    : mLogicalDevice(logicalDevice)
{}

ShaderModuleSystem::~ShaderModuleSystem() {
    clear();
}

const ShaderModule&
ShaderModuleSystem::getOrLoadShaderModule(const std::string& shaderByteCodePath,
                                          const VkShaderStageFlagBits shaderStageFlag,
                                          const char* entryPointName) {
    const ShaderModule* shaderModule = nullptr;

    // Check if the shader module was already loaded.
    ShaderModuleByPath::const_iterator findIt = mShaderModuleByPath.find(shaderByteCodePath);
    if (findIt != mShaderModuleByPath.end()) {
        shaderModule = findIt->second;
        assert(shaderModule != nullptr);
        assert(strcmp(entryPointName, shaderModule->entryPointName()) == 0);
        assert(shaderStageFlag == shaderModule->shaderStageFlag());
    } else {
        shaderModule = new ShaderModule(mLogicalDevice,
                                        shaderByteCodePath,
                                        shaderStageFlag,
                                        entryPointName);

        mShaderModuleByPath[shaderByteCodePath] = shaderModule;
    }

    assert(shaderModule != nullptr);

    return *shaderModule;
}

void
ShaderModuleSystem::eraseShaderModule(const std::string& shaderByteCodePath) {
    ShaderModuleByPath::const_iterator findIt = mShaderModuleByPath.find(shaderByteCodePath);
    if (findIt != mShaderModuleByPath.end()) {
        const ShaderModule* shaderModule = findIt->second;
        assert(shaderModule != nullptr);
        delete shaderModule;
        mShaderModuleByPath.erase(findIt);
    }
}

void 
ShaderModuleSystem::clear() {
    for (const auto& pathAndShaderModule : mShaderModuleByPath) {
        delete pathAndShaderModule.second;
    }
}
}