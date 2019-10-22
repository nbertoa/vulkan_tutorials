#ifndef UTILS_SHADER_MODULE
#define UTILS_SHADER_MODULE

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
// Assumptions:
// The entry point of the shader must be "main"
//
class ShaderModule {
public:
    // * shaderByteCodePath to the file that represents the piece of shader code, 
    //   in SPIR-V format.
    //
    // * shaderStageFlag specifies the pipeline stage:
    //   - VK_SHADER_STAGE_VERTEX_BIT
    //   - VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
    //   - VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
    //   - VK_SHADER_STAGE_GEOMETRY_BIT 
    //   - VK_SHADER_STAGE_FRAGMENT_BIT
    //   - VK_SHADER_STAGE_COMPUTE_BIT
    //   - VK_SHADER_STAGE_TASK_BIT_NV
    //   - VK_SHADER_STAGE_MESH_BIT_NV
    //   - VK_SHADER_STAGE_ALL_GRAPHICS is a combination of bits used as shorthand 
    //     to specify all graphics stages defined above(excluding the compute stage).
    //   - VK_SHADER_STAGE_ALL is a combination of bits used as shorthand to specify 
    //     all shader stages supported by the device, including all additional stages 
    //     which are introduced by extensions.
    //   - VK_SHADER_STAGE_RAYGEN_BIT_NV specifies the ray generation stage.
    //   - VK_SHADER_STAGE_ANY_HIT_BIT_NV
    //   - VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV
    //   - VK_SHADER_STAGE_MISS_BIT_NV
    //   - VK_SHADER_STAGE_INTERSECTION_BIT_NV
    //   - VK_SHADER_STAGE_CALLABLE_BIT_NV
    ShaderModule(const LogicalDevice& logicalDevice,
                 const std::string& shaderByteCodePath,
                 const VkShaderStageFlagBits shaderStageFlag);
    ~ShaderModule();
    ShaderModule(ShaderModule&& other) noexcept;
    ShaderModule(const ShaderModule&) = delete;
    const ShaderModule& operator=(const ShaderModule&) = delete;

    // VkPipelineShaderStageCreateInfo:
    // - stage specifies a single pipeline stage:
    //   . VK_SHADER_STAGE_VERTEX_BIT
    //   . VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
    //   . VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
    //   . VK_SHADER_STAGE_GEOMETRY_BIT
    //   . VK_SHADER_STAGE_FRAGMENT_BIT
    //   . VK_SHADER_STAGE_COMPUTE_BIT
    //   . VK_SHADER_STAGE_TASK_BIT_NV.
    //   . VK_SHADER_STAGE_MESH_BIT_NV
    //   . VK_SHADER_STAGE_ALL_GRAPHICS is a combination of bits used as shorthand to specify all 
    //     graphics stages defined above(excluding the compute stage).
    //   . VK_SHADER_STAGE_ALL is a combination of bits used as shorthand to specify all shader stages 
    //     supported by the device, including all additional stages which are introduced by extensions.
    //   . VK_SHADER_STAGE_RAYGEN_BIT_NV specifies the ray generation stage.
    //   . VK_SHADER_STAGE_ANY_HIT_BIT_NV
    //   . VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV
    //   . VK_SHADER_STAGE_MISS_BIT_NV
    //   . VK_SHADER_STAGE_INTERSECTION_BIT_NV
    //   . VK_SHADER_STAGE_CALLABLE_BIT_NV
    // - module containing the shader for this stage.
    // - pName specifies the entry point name of the shader for this stage ("main in our case")
    VkPipelineShaderStageCreateInfo 
    shaderStage() const;

    const std::string& 
    shaderByteCodePath() const;

    VkShaderStageFlagBits 
    shaderStageFlag() const;

private:
    static std::vector<char> 
    readFile(const std::string& shaderByteCodePath);

    VkShaderModule 
    createShaderModule();

    const LogicalDevice& mLogicalDevice;
    VkShaderStageFlagBits mShaderStageFlag;
    std::string mShaderByteCodePath;
    VkShaderModule mShaderModule = VK_NULL_HANDLE;
};
}


#endif