#ifndef UTILS_SHADER_MODULE
#define UTILS_SHADER_MODULE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkShaderModule wrapper to be able to create/destroy/get/handle shader module easily.
// VkShaderModule is just a thin wrapper around the shader bytecode that 
// we have previously loaded from a file and the functions defined in it.
class ShaderModule {
public:
    ShaderModule(const char* shaderFilepath, 
                 const LogicalDevice& logicalDevice);
    ~ShaderModule();

private:
    static std::vector<char> readFile(const char* shaderByteCodePath);

    void createShaderModule();

    const LogicalDevice& mLogicalDevice;
    const char* mShaderByteCodePath = nullptr;
    VkShaderModule mShaderModule = VK_NULL_HANDLE;
};
}

#endif