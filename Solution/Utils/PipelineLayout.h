#ifndef UTILS_PIPELINE_LAYOUT
#define UTILS_PIPELINE_LAYOUT

#include <vector>
#include <vulkan/vulkan.h>

#include "DescriptorSetLayout.h"

namespace vk {
class LogicalDevice;

//
// VkPipelineLayout wrapper.
//
// PipelineLayout represents a configuration of the rendering pipeline in terms of what types 
// of descriptor sets will be bound to the CommandBuffer. 
// Access to descriptor sets from a pipeline is accomplished through a pipeline layout.
//
// Zero or more descriptor set layouts and zero or more push constant ranges are combined to 
// form a pipeline layout object describing the complete set of resources that can be accessed 
// by a pipeline.
//
// The pipeline layout represents a sequence of descriptor sets with each having a specific layout.
// This sequence of layouts is used to determine the interface between shader stages and shader resources.
// Each pipeline is created using a pipeline layout.
//
// You need the PipelineLayout to:
// - Bind DescriptorSets to a CommandBuffer through vkCmdBindDescriptorSets
//
// To create/use the PipelineLayout you need:
// - DescriptorSetLayout
//
class PipelineLayout {
public:
    // * descriptorSetLayout: Read DescriptorSetLayout class comments for more information about this.
    //
    // * pushConstantRange:
    //
    //   - stageFlags describing the shader stages that will access a range 
    //     of push constants. If a particular stage is not included in the range, then accessing members 
    //     of that range of push constants from the corresponding shader stage will return undefined values.
    //   - offset and size are the start offset and size, respectively, consumed by the range.
    //     Both offset and size are in units of bytes and must be a multiple of 4. 
    //     The layout of the push constant variables is specified in the shader.
    PipelineLayout(const LogicalDevice& logicalDevice,
                   const DescriptorSetLayout* const descriptorSetLayout = nullptr,
                   const VkPushConstantRange* const pushConstantRange = nullptr);
    ~PipelineLayout();
    PipelineLayout(PipelineLayout&& other) noexcept;
    PipelineLayout(const PipelineLayout&) = delete;
    const PipelineLayout& operator=(const PipelineLayout&) = delete;

    const VkPipelineLayout& 
    pipelineLayout() const;

private:
    void 
    createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                         const std::vector<VkPushConstantRange>& pushConstantRanges);

    const LogicalDevice& mLogicalDevice;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
};
}

#endif