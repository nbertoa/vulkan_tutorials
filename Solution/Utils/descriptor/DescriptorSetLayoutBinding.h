#ifndef UTILS_DESCRIPTOR_DESCRIPTOR_SET_LAYOUT_BINDING
#define UTILS_DESCRIPTOR_DESCRIPTOR_SET_LAYOUT_BINDING

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
//
// VkDescriptorSetBinding wrapper.
//
// The way shaders can access these resources (Buffers, Images and Samplers) is through descriptors. 
// 
// Descriptors do not exist on their own, but are always grouped in descriptor sets. 
// A descriptor set object is an opaque object that contains storage for a 
// set of descriptors, where the types and number of descriptors is defined 
// by a descriptor set layout. 
//
// But before you create a descriptor set, its layout must be specified by creating a DescriptorSetLayout, 
// which behaves like a template for a descriptor set. 
// The layout object may be used to define the association of each descriptor 
// binding with memory or other hardware resources.
//
// The layout is used both for determining the resources that need to be 
// associated with the descriptor set, and determining the interface between 
// shader stages and shader resources.
// For example, your shaders used by a rendering pass for drawing 3D geometry may expect:
// 
// Binding Slot   Resource
//      0            One uniform buffer (cbuffer in D3D) available to the vertex shader stage.
//      1            Another uniform buffer available to the fragment shader stage.
//      2            A sampled image.
//      3            A sampler, also available to the fragment shader stage.
//
// A descriptor set layout object is defined by an array of 
// zero or more descriptor bindings. 
// Each individual descriptor binding is specified by a descriptor type, 
// a count (array size) of the number of descriptors in the binding, 
// a set of shader stages that can access the binding, 
// and (if using immutable samplers) an array of sampler descriptors.
//
// You need the DescriptorSetLayoutBinding to:
// - Create the DescriptorSetLayout
//
class DescriptorSetLayoutBinding {
public:
    // * bindingNumber of this entry and corresponds to a resource 
    //   of the same binding number in the shader stages.
    //
    // * descriptorType of resource descriptors are used for this binding:
    //
    //     - VK_DESCRIPTOR_TYPE_SAMPLER
    //     - VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    //     - VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
    //     - VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
    //     - VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
    //     - VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
    //     - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    //     - VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    //     - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    //     - VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
    //     - VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
    //     - VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT
    //
    // * descriptorCount contained in the binding, 
    //   accessed in a shader as an array, except if descriptorType is 
    //   VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT in which case descriptorCount 
    //   is the size in bytes of the inline uniform block.
    //   If descriptorCount is zero this binding entry is reserved and the resource 
    //   must not be accessed from any stage via this binding within 
    //   any pipeline using the set layout.
    //
    // * stageFlags bitmask that specifies which pipeline shader stages can access a resource for this binding.
    //   VK_SHADER_STAGE_ALL is a shorthand specifying that all defined shader stages, 
    //   including any additional stages defined by extensions, can access the resource.
    //   If a shader stage is not included in stageFlags, then a resource must not be 
    //   accessed from that stage via this binding within any pipeline using the set layout.
    //   Other than input attachments which are limited to the fragment shader, 
    //   there are no limitations on what combinations of stages can use a descriptor binding, 
    //   and in particular a binding can be used by both graphics stages and the compute stage:
    //
    //     - VK_SHADER_STAGE_VERTEX_BIT
    //     - VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
    //     - VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
    //     - VK_SHADER_STAGE_GEOMETRY_BIT
    //     - VK_SHADER_STAGE_FRAGMENT_BIT
    //     - VK_SHADER_STAGE_COMPUTE_BIT
    //     - VK_SHADER_STAGE_TASK_BIT_NV
    //     - VK_SHADER_STAGE_MESH_BIT_NV
    //     - VK_SHADER_STAGE_ALL_GRAPHICS is a combination of bits used as shorthand to specify 
    //       all graphics stages defined above(excluding the compute stage).
    //     - VK_SHADER_STAGE_ALL is a combination of bits used as shorthand to specify all shader 
    //       stages supported by the device, including all additional stages which are introduced by extensions.
    //     - VK_SHADER_STAGE_RAYGEN_BIT_NV specifies the ray generation stage.
    //     - VK_SHADER_STAGE_ANY_HIT_BIT_NV
    //     - VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV
    //     - VK_SHADER_STAGE_MISS_BIT_NV
    //     - VK_SHADER_STAGE_INTERSECTION_BIT_NV
    //     - VK_SHADER_STAGE_CALLABLE_BIT_NV
    //
    // * immutableSamplers affects initialization of samplers. 
    //   If descriptorType specifies a VK_DESCRIPTOR_TYPE_SAMPLER or VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER type descriptor, 
    //   then pImmutableSamplers can be used to initialize a set of immutable samplers. 
    //   Immutable samplers are permanently bound into the set layout and must not be changed; 
    //   updating a VK_DESCRIPTOR_TYPE_SAMPLER descriptor with immutable samplers is not allowed and updates to a 
    //   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER descriptor with immutable samplers does not modify the samplers 
    //   (the image views are updated, but the sampler updates are ignored). If immutableSamplers is not nullptr, 
    //   then it points to an array of sampler handles that will be copied into the set layout and used for the corresponding binding. 
    //   Only the sampler handles are copied; the sampler objects must not be destroyed before the final use of the set layout 
    //   and any descriptor pools and sets created using it. If immutableSamplers is nullptr, 
    //   then the sampler slots are dynamic and sampler handles must be bound into descriptor sets using this layout. 
    //   If descriptorType is not one of these descriptor types, then immutableSamplers is ignored.
    DescriptorSetLayoutBinding(const uint32_t bindingNumber,
                               const VkDescriptorType descriptorType,
                               const uint32_t descriptorCount,
                               const VkShaderStageFlags stageFlags,
                               const std::vector<VkSampler>& immutableSamplers = {});

private:
    VkDescriptorSetLayoutBinding mDescriptorSetLayoutBinding = {};
};
}

#endif
