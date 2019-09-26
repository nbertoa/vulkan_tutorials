#ifndef UTILS_PIPELINE_LAYOUT
#define UTILS_PIPELINE_LAYOUT

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class DescriptorSetLayout;
class LogicalDevice;

// VkPipelineLayout wrapper.
// Access to descriptor sets from a pipeline is accomplished through a pipeline layout.
// Zero or more descriptor set layouts and zero or more push constant ranges are combined to 
// form a pipeline layout object describing the complete set of resources that can be accessed 
// by a pipeline.
// The pipeline layout represents a sequence of descriptor sets with each having a specific layout.
// This sequence of layouts is used to determine the interface between shader stagesand shader resources.
// Each pipeline is created using a pipeline layout.
class PipelineLayout {
public:
    // Descriptor layouts:
    // A descriptor set layout object is defined by an array of zero or more descriptor bindings. 
    // Each individual descriptor binding is specified by a descriptor type, a count (array size) of 
    // the number of descriptors in the binding, a set of shader stages that can access the binding, 
    // and (if using immutable samplers) an array of sampler descriptors. 
    //
    // Push constant range:
    // - stageFlags is a set of stage flags describing the shader stages that will access a range 
    // of push constants. If a particular stage is not included in the range, then accessing members 
    // of that range of push constants from the corresponding shader stage will return undefined values.
    // - offset and size are the start offset and size, respectively, consumed by the range.
    // Both offsetand size are in units of bytesand must be a multiple of 4. 
    // The layout of the push constant variables is specified in the shader.
    PipelineLayout(const LogicalDevice& logicalDevice,
                   const DescriptorSetLayout* descriptorSetLayout,
                   const VkPushConstantRange* pushConstantRange);
    PipelineLayout(const LogicalDevice& logicalDevice,
                   const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts = std::vector<VkDescriptorSetLayout>(),
                   const std::vector<VkPushConstantRange>& pushConstantRanges = std::vector<VkPushConstantRange>());
    ~PipelineLayout();
    PipelineLayout(PipelineLayout&& other) noexcept;
    PipelineLayout(const PipelineLayout&) = delete;
    const PipelineLayout& operator=(const PipelineLayout&) = delete;

    const VkPipelineLayout& pipelineLayout() const;

private:
    void createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                              const std::vector<VkPushConstantRange>& pushConstantRanges);

    const LogicalDevice& mLogicalDevice;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
};
}

#endif