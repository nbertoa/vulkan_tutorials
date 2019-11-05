#ifndef UTILS_DESCRIPTOR_DESCRIPTOR_SET_LAYOUT
#define UTILS_DESCRIPTOR_DESCRIPTOR_SET_LAYOUT

#include <vector>
#include <vulkan/vulkan.h>

#include "DescriptorSetLayoutBinding.h"

namespace vk {
class LogicalDevice;

//
// VkDescriptorSetLayout wrapper.
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
// You need the DescriptorSetLayout to:
// - Create the PipelineLayout.
// - Create the DescriptorSet
//
class DescriptorSetLayout {
public:
    // * descriptorSetLayoutBindings for the descriptor set layout.
    DescriptorSetLayout(const LogicalDevice& logicalDevice,
                        const std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
    ~DescriptorSetLayout();
    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    const DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    VkDescriptorSetLayout 
    vkDescriptorSetLayout() const;

private:
    const LogicalDevice& mLogicalDevice;
    VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
};
}

#endif