#ifndef UTILS_SYSTEM_SHADER_MODULE_SYSTEM
#define UTILS_SYSTEM_SHADER_MODULE_SYSTEM

#include <unordered_map>
#include "../ShaderModule.h"

namespace vk {
class LogicalDevice;

class ShaderModuleSystem {
public:
    ShaderModuleSystem(const LogicalDevice& logicalDevice);
    ~ShaderModuleSystem();
    ShaderModuleSystem(ShaderModuleSystem&&) noexcept = delete;
    ShaderModuleSystem(const ShaderModuleSystem&) = delete;
    const ShaderModuleSystem& operator=(const ShaderModuleSystem&) = delete;

    const ShaderModule& 
    getOrLoadShaderModule(const std::string& shaderByteCodePath,
                          const VkShaderStageFlagBits shaderStageFlag,
                          const char* entryPointName = "main");

private:
    const LogicalDevice& mLogicalDevice;
    using ShaderModuleByPath = std::unordered_map<std::string, const ShaderModule*>;
    ShaderModuleByPath mShaderModuleByPath;

};
}

#endif
