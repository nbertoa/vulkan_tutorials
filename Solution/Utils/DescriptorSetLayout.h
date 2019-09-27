#ifndef UTILS_DESCRIPTOR_SET_LAYOUT
#define UTILS_DESCRIPTOR_SET_LAYOUT

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkDescriptorSetLayout wrapper.
// Descriptors are grouped together into descriptor set objects. 
// A descriptor set object is an opaque object that contains storage for a 
// set of descriptors, where the types and number of descriptors is defined 
// by a descriptor set layout. 
// The layout object may be used to define the association of each descriptor 
// binding with memory or other hardware resources. 
// The layout is used both for determining the resources that need to be 
// associated with the descriptor set, and determining the interface between 
// shader stages and shader resources.
// A descriptor set layout object is defined by an array of 
// zero or more descriptor bindings. 
// Each individual descriptor binding is specified by a descriptor type, 
// a count (array size) of the number of descriptors in the binding, 
// a set of shader stages that can access the binding, 
// and (if using immutable samplers) an array of sampler descriptors.
class DescriptorSetLayout {
    public:
        // VkDescriptorSetLayoutBinding:
        // - binding is the binding number of this entry and corresponds to a resource 
        //   of the same binding number in the shader stages.
        // - descriptorType is a VkDescriptorType specifying which type of 
        //   resource descriptors are used for this binding.
        //     - VK_DESCRIPTOR_TYPE_SAMPLER specifies a sampler descriptor.
        //     - VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER specifies a combined image sampler descriptor.
        //     - VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE specifies a sampled image descriptor.
        //     - VK_DESCRIPTOR_TYPE_STORAGE_IMAGE specifies a storage image descriptor.
        //     - VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER specifies a uniform texel buffer descriptor.
        //     - VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER specifies a storage texel buffer descriptor.
        //     - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER specifies a uniform buffer descriptor.
        //     - VK_DESCRIPTOR_TYPE_STORAGE_BUFFER specifies a storage buffer descriptor.
        //     - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC specifies a dynamic uniform buffer descriptor.
        //     - VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC specifies a dynamic storage buffer descriptor.
        //     - VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT specifies an input attachment descriptor.
        //     - VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT specifies an inline uniform block.
        // - descriptorCount is the number of descriptors contained in the binding, 
        //   accessed in a shader as an array, except if descriptorType is 
        //   VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT in which case descriptorCount 
        //   is the size in bytes of the inline uniform block.
        //   If descriptorCount is zero this binding entry is reserved and the resource 
        //   must not be accessed from any stage via this binding within 
        //   any pipeline using the set layout.
        // - stageFlags member is a bitmask of VkShaderStageFlagBits specifying 
        //   which pipeline shader stages can access a resource for this binding.
        //   VK_SHADER_STAGE_ALL is a shorthand specifying that all defined shader stages, 
        //   including any additional stages defined by extensions, can access the resource.
        //   If a shader stage is not included in stageFlags, then a resource must not be 
        //   accessed from that stage via this binding within any pipeline using the set layout.
        //   Other than input attachments which are limited to the fragment shader, 
        //   there are no limitations on what combinations of stages can use a descriptor binding, 
        //   and in particular a binding can be used by both graphics stages and the compute stage.
        //     - VK_SHADER_STAGE_VERTEX_BIT specifies the vertex stage.
        //     - VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT specifies the tessellation control stage.
        //     - VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT specifies the tessellation evaluation stage.
        //     - VK_SHADER_STAGE_GEOMETRY_BIT specifies the geometry stage.
        //     - VK_SHADER_STAGE_FRAGMENT_BIT specifies the fragment stage.
        //     - VK_SHADER_STAGE_COMPUTE_BIT specifies the compute stage.
        //     - VK_SHADER_STAGE_TASK_BIT_NV specifies the task stage.
        //     - VK_SHADER_STAGE_MESH_BIT_NV specifies the mesh stage.
        //     - VK_SHADER_STAGE_ALL_GRAPHICS is a combination of bits used as shorthand to specify 
        //       all graphics stages defined above(excluding the compute stage).
        //     - VK_SHADER_STAGE_ALL is a combination of bits used as shorthand to specify all shader 
        //       stages supported by the device, including all additional stages which are introduced by extensions.
        //     - VK_SHADER_STAGE_RAYGEN_BIT_NV specifies the ray generation stage.
        //     - VK_SHADER_STAGE_ANY_HIT_BIT_NV specifies the any - hit stage.
        //     - VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV specifies the closest hit stage.
        //     - VK_SHADER_STAGE_MISS_BIT_NV specifies the miss stage.
        //     - VK_SHADER_STAGE_INTERSECTION_BIT_NV specifies the intersection stage.
        //     - VK_SHADER_STAGE_CALLABLE_BIT_NV specifies the callable stage.
        DescriptorSetLayout(const LogicalDevice& logicalDevice,
                            const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
        ~DescriptorSetLayout();
        DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        const DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

        VkDescriptorSetLayout vkDescriptorSetLayout() const;

private:
    const LogicalDevice& mLogicalDevice;
    VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
};
}

#endif