#include "PipelineLayout.h"

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
PipelineLayout::PipelineLayout(const LogicalDevice& logicalDevice,
                               const VkDescriptorSetLayout& descriptorSetLayout,
                               const VkPushConstantRange& pushConstantRange)
    : mLogicalDevice(logicalDevice)
{
    createPipelineLayout({descriptorSetLayout},
                         {pushConstantRange});
}

PipelineLayout::PipelineLayout(const LogicalDevice& logicalDevice,
                               const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                               const std::vector<VkPushConstantRange>& pushConstantRanges)
    : mLogicalDevice(logicalDevice)
{
    createPipelineLayout(descriptorSetLayouts,
                         pushConstantRanges);
}

PipelineLayout::~PipelineLayout() {
    assert(mPipelineLayout != VK_NULL_HANDLE);
    vkDestroyPipelineLayout(mLogicalDevice.vkDevice(),
                            mPipelineLayout,
                            nullptr);
}

void
PipelineLayout::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                                     const std::vector<VkPushConstantRange>& pushConstantRanges) {
    assert(mPipelineLayout == VK_NULL_HANDLE);

    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    createInfo.pSetLayouts = descriptorSetLayouts.empty() ? nullptr : descriptorSetLayouts.data();
    createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    createInfo.pPushConstantRanges = pushConstantRanges.empty() ? nullptr : pushConstantRanges.data();

    vkChecker(vkCreatePipelineLayout(mLogicalDevice.vkDevice(),
                                     &createInfo,
                                     nullptr,
                                     &mPipelineLayout));
    assert(mPipelineLayout != VK_NULL_HANDLE);
}
}