#ifndef UTILS_PIPELINE_LAYOUT
#define UTILS_PIPELINE_LAYOUT

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkPipelineLayout wrapper to create/destroy/get it easily.
// We need to use it to specify uniform shader values
// during pipeline creation.
class PipelineLayout {
public:
    PipelineLayout(const LogicalDevice& logicalDevice,
                   const VkDescriptorSetLayout& descriptorSetLayout,
                   const VkPushConstantRange& pushConstantRange);
    PipelineLayout(const LogicalDevice& logicalDevice,
                   const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts = std::vector<VkDescriptorSetLayout>(),
                   const std::vector<VkPushConstantRange>& pushConstantRanges = std::vector<VkPushConstantRange>());
    ~PipelineLayout();

    PipelineLayout(const PipelineLayout&) = delete;
    const PipelineLayout& operator=(const PipelineLayout&) = delete;

    const VkPipelineLayout& pipelineLayout() const {
        assert(mPipelineLayout != VK_NULL_HANDLE);
        return mPipelineLayout;
    }

private:
    void createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                              const std::vector<VkPushConstantRange>& pushConstantRanges);

    const LogicalDevice& mLogicalDevice;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
};
}

#endif