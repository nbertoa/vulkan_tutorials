#include "PipelineLayout.h"

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
PipelineLayout::PipelineLayout(const LogicalDevice& logicalDevice,
                               const VkDescriptorSetLayout& descriptorSetLayout,
                               const VkPushConstantRange& pushConstantRange)
    : mLogicalDevice(logicalDevice)
    , mDescriptorSetLayouts{ descriptorSetLayout }
    , mPushConstantRanges{ pushConstantRange }
{
    createPipelineLayout();
}

PipelineLayout::PipelineLayout(const LogicalDevice& logicalDevice,
                               const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                               const std::vector<VkPushConstantRange>& pushConstantRanges)
    : mLogicalDevice(logicalDevice)
    , mDescriptorSetLayouts(descriptorSetLayouts)
    , mPushConstantRanges(pushConstantRanges)
{
    createPipelineLayout();
}

PipelineLayout::~PipelineLayout() {
    assert(mPipelineLayout != VK_NULL_HANDLE);
    vkDestroyPipelineLayout(mLogicalDevice.vkDevice(),
                            mPipelineLayout,
                            nullptr);
}

void
PipelineLayout::createPipelineLayout() {
    assert(mPipelineLayout == VK_NULL_HANDLE);

    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = static_cast<uint32_t>(mDescriptorSetLayouts.size());
    createInfo.pSetLayouts = mDescriptorSetLayouts.empty() ? nullptr : mDescriptorSetLayouts.data();
    createInfo.pushConstantRangeCount = static_cast<uint32_t>(mPushConstantRanges.size());
    createInfo.pPushConstantRanges = mPushConstantRanges.empty() ? nullptr : mPushConstantRanges.data();

    vkChecker(vkCreatePipelineLayout(mLogicalDevice.vkDevice(),
                                     &createInfo,
                                     nullptr,
                                     &mPipelineLayout));
    assert(mPipelineLayout != VK_NULL_HANDLE);
}
}