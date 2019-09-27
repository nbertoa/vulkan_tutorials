#ifndef UTILS_SHADER_MODULE
#define UTILS_SHADER_MODULE

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkShaderModule wrapper.
// VkShaderModule is just a thin wrapper around the shader bytecode that 
// we have previously loaded from a file and the functions defined in it.
//
// Shader modules contain shader code and one or more entry points.
// Shaders are selected from a shader module by specifying an entry 
// point as part of pipeline creation.
// The stages of a pipeline can use shaders that come from different modules.
// The shader code defining a shader module must be in the SPIR - V format.
class ShaderModule {
public:
    // shaderStageFlag indicates the shader type.
    // Preconditions:
    // The entry point of the shader must be "main"
    //
    // flags:
    // - VK_SHADER_STAGE_VERTEX_BIT specifies the vertex stage.
    // - VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT specifies the tessellation control stage.
    // - VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT specifies the tessellation evaluation stage.
    // - VK_SHADER_STAGE_GEOMETRY_BIT specifies the geometry stage.
    // - VK_SHADER_STAGE_FRAGMENT_BIT specifies the fragment stage.
    // - VK_SHADER_STAGE_COMPUTE_BIT specifies the compute stage.
    // - VK_SHADER_STAGE_TASK_BIT_NV specifies the task stage.
    // - VK_SHADER_STAGE_MESH_BIT_NV specifies the mesh stage.
    // - VK_SHADER_STAGE_ALL_GRAPHICS is a combination of bits used as shorthand 
    //   to specify all graphics stages defined above(excluding the compute stage).
    // - VK_SHADER_STAGE_ALL is a combination of bits used as shorthand to specify 
    //   all shader stages supported by the device, including all additional stages 
    //   which are introduced by extensions.
    // - VK_SHADER_STAGE_RAYGEN_BIT_NV specifies the ray generation stage.
    // - VK_SHADER_STAGE_ANY_HIT_BIT_NV specifies the any - hit stage.
    // - VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV specifies the closest hit stage.
    // - VK_SHADER_STAGE_MISS_BIT_NV specifies the miss stage.
    // - VK_SHADER_STAGE_INTERSECTION_BIT_NV specifies the intersection stage.
    // - VK_SHADER_STAGE_CALLABLE_BIT_NV specifies the callable stage.
    ShaderModule(const LogicalDevice& logicalDevice,
                 const std::string& shaderFilepath,
                 const VkShaderStageFlagBits shaderStageFlag);
    ~ShaderModule();
    ShaderModule(ShaderModule&& other) noexcept;
    ShaderModule(const ShaderModule&) = delete;
    const ShaderModule& operator=(const ShaderModule&) = delete;

    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo() const;

    const std::string& shaderByteCodePath() const;

    VkShaderStageFlagBits shaderStageFlag() const;

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