#ifndef UTILS_PIPELINE_STAGE_SHADER_STAGES
#define UTILS_PIPELINE_STAGE_SHADER_STAGES

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class ShaderModule;

//
// VkPipelineShaderStageCreateInfo vector wrapper
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
// do you specify the shader module plus the name of the entry point function (like “main”).
//
// You need this class to:
// - Create the GraphicsPipeline
//
class ShaderStages {
public:
    // * shaderModule (Read ShaderModule comments to understand this parameter)
    void 
    addShaderModule(const ShaderModule& shaderModule);

    const std::vector<VkPipelineShaderStageCreateInfo>&
    vkStages() const;

private:
    std::vector<VkPipelineShaderStageCreateInfo> mCreateInfoVec;
};
}

#endif 