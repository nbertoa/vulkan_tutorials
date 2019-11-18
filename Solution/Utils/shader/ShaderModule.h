#ifndef UTILS_SHADER_SHADER_MODULE
#define UTILS_SHADER_SHADER_MODULE

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

//
// VkShaderModule wrapper.
//
// Shader compilation is a multi-stage process in Vulkan. First, Vulkan does not support any
// high-level shading language like GLSL or HLSL. 
// Instead, Vulkan accepts an intermediate format called SPIR-V which any higher-level language can emit.
//
// A buffer filled with data in SPIR-V is used to create a ShaderModule.
// This object represents a piece of shader code, possibly in some partially compiled form, 
// but it is not anything the GPU can execute yet.
//
// Only when creating the Pipeline for each shader stage you are going to use
// (vertex, tessellation control, tessellation evaluation, geometry, fragment, or compute) 
// do you specify the ShaderModule plus the name of the entry point function (like “main”).
//
// VkShaderModule is just a thin wrapper around the shader bytecode that 
// we have previously loaded from a file and the functions defined in it.
//
// You need the ShaderModule to:
// - Create the Pipeline (calling pipelineShaderStageCreateInfo())
//
class ShaderModule {
public:
    // * shaderByteCodePath to the file that represents the piece of shader code, 
    //   in SPIR-V format.
    //
    // * shaderStageFlag specifies the pipeline stage S(VK_SHADER_STAGE_):
    //
    //   - VERTEX_BIT, TESSELLATION_CONTROL_BIT, TESSELLATION_EVALUATION_BIT, GEOMETRY_BIT,
    //     FRAGMENT_BIT, COMPUTE_BIT, TASK_BIT_NV, MESH_BIT_NV, ALL_GRAPHICS, ALL, 
    //     RAYGEN_BIT_NV, ANY_HIT_BIT_NV, CLOSEST_HIT_BIT_NV, MISS_BIT_NV, INTERSECTION_BIT_NV, 
    //     CALLABLE_BIT_NV
    ShaderModule(const LogicalDevice& logicalDevice,
                 const std::string& shaderByteCodePath,
                 const VkShaderStageFlagBits shaderStageFlag,
                 const char* entryPointName = "main");
    ~ShaderModule();
    ShaderModule(ShaderModule&& other) noexcept;
    ShaderModule(const ShaderModule&) = delete;
    const ShaderModule& operator=(const ShaderModule&) = delete;

    const std::string& 
    shaderByteCodePath() const;

    VkShaderStageFlagBits 
    shaderStageFlag() const;

    VkShaderModule
    vkShaderModule() const;

    const char* 
    entryPointName() const;

private:
    static std::vector<char> 
    readFile(const std::string& shaderByteCodePath);

    VkShaderModule 
    createShaderModule();

    const LogicalDevice& mLogicalDevice;
    VkShaderStageFlagBits mShaderStageFlag;
    std::string mShaderByteCodePath;
    VkShaderModule mShaderModule = VK_NULL_HANDLE;
    const char* mEntryPointName = nullptr;
};
}


#endif