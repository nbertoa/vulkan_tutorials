#ifndef UTILS_SHADER_SHADER_MODULE
#define UTILS_SHADER_SHADER_MODULE

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk2 {
//
// VkShaderModule wrapper.
//
// A shader specifies programmable operations that execute for each vertex, 
// control point, tessellated vertex, primitive, fragment, or workgroup in the 
// corresponding stage(s) of the graphics and compute pipelines.
//
// Shaders can read from input variables, and read from and write to output variables.
//
// Input and output variables can be used to transfer data between shader stages, 
// or to allow the shader to interact with values that exist in the execution environment.
//
// Similarly, the execution environment provides constants that describe capabilities.
//
// Shader variables are associated with execution environment-provided inputs 
// and outputs using built-in decorations in the shader.
//
// Shaders are selected from a shader module by specifying an entry point as 
// part of pipeline creation.The stages of a pipeline can
// use shaders that come from different modules.
//
// Shader modules contain shader codeand one or more entry points.
//
// Shaders are selected from a shader module by specifying an entry point as part of 
// pipeline creation.
//
// The stages of a pipeline can use shaders that come from different modules.
//
// The shader code defining a shader module must be in the SPIR-V format
//
// Once a shader module has been created, any entry points it contains 
// can be used in pipeline shader stages.
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
    //
    // Notes: The global logical devices creates the shader module.
    ShaderModule(const std::string& shaderByteCodePath,
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

    VkShaderStageFlagBits mShaderStageFlag;
    std::string mShaderByteCodePath;
    VkShaderModule mShaderModule = VK_NULL_HANDLE;
    const char* mEntryPointName = nullptr;
};
}


#endif