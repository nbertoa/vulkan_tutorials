#ifndef UTILS_SHADER_SHADER_MODULE_SYSTEM
#define UTILS_SHADER_SHADER_MODULE_SYSTEM

#include <unordered_map>

#include "ShaderModule.h"

namespace vk2 {

class ShaderModuleSystem {
public:
    static const ShaderModule& 
    getOrLoadShaderModule(const std::string& shaderByteCodePath,
                          const vk::ShaderStageFlagBits shaderStageFlag,
                          const char* entryPointName = "main");

    static void
    eraseShaderModule(const std::string& shaderByteCodePath);

    static void 
    clear();

private:
    ShaderModuleSystem() = delete;
    ~ShaderModuleSystem() = delete;
    ShaderModuleSystem(ShaderModuleSystem&&) noexcept = delete;
    ShaderModuleSystem(const ShaderModuleSystem&) = delete;
    const ShaderModuleSystem& operator=(const ShaderModuleSystem&) = delete;

    using ShaderModuleByPath = std::unordered_map<std::string, const ShaderModule*>;
    static ShaderModuleByPath mShaderModuleByPath;
};
}

#endif
