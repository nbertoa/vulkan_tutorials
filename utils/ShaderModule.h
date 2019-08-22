#ifndef UTILS_SHADER_MODULE
#define UTILS_SHADER_MODULE

#include <cassert>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkShaderModule wrapper to be able to create/destroy/get/handle 
// shader module easily.
// VkShaderModule is just a thin wrapper around the shader bytecode that 
// we have previously loaded from a file and the functions defined in it.
// We assume the entry point of the shader is "main".
class ShaderModule {
public:
    // shaderStageFlag indicates the shader type.
    ShaderModule(const LogicalDevice& logicalDevice,
                 const std::string& shaderFilepath,
                 const VkShaderStageFlagBits shaderStageFlag);
    ~ShaderModule();

    ShaderModule(const ShaderModule&) = delete;
    const ShaderModule& operator=(const ShaderModule&) = delete;

    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo() const;

    const std::string& shaderByteCodePath() const { 
        assert(mShaderModule != VK_NULL_HANDLE);
        return mShaderByteCodePath;
    }

    VkShaderStageFlagBits shaderStageFlag() const {
        assert(mShaderModule != VK_NULL_HANDLE);
        return mShaderStageFlag;
    }

private:
    static std::vector<char> readFile(const std::string& shaderByteCodePath);

    void createShaderModule();

    const LogicalDevice& mLogicalDevice;
    VkShaderStageFlagBits mShaderStageFlag;
    std::string mShaderByteCodePath;
    VkShaderModule mShaderModule = VK_NULL_HANDLE;
};
}


#endif