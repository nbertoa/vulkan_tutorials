#include "PipelineLayout.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk {
PipelineLayout::PipelineLayout(const DescriptorSetLayout* const descriptorSetLayout,
                               const VkPushConstantRange* const pushConstantRange) {
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    if (descriptorSetLayout != nullptr) {
        descriptorSetLayouts.push_back(descriptorSetLayout->vkDescriptorSetLayout());
    }
    std::vector<VkPushConstantRange> pushConstantRanges;
    if (pushConstantRange != nullptr) {
        pushConstantRanges.push_back(*pushConstantRange);
    }
    createPipelineLayout(descriptorSetLayouts,
                         pushConstantRanges);
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(LogicalDevice::vkDevice(),
                            mPipelineLayout,
                            nullptr);
}

PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
    : mPipelineLayout(other.mPipelineLayout)
{
    other.mPipelineLayout = VK_NULL_HANDLE;
}

const VkPipelineLayout& 
PipelineLayout::vkPipelineLayout() const {
    assert(mPipelineLayout != VK_NULL_HANDLE);
    return mPipelineLayout;
}

void
PipelineLayout::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                                     const std::vector<VkPushConstantRange>& pushConstantRanges) {
    assert(mPipelineLayout == VK_NULL_HANDLE);

    // VkPipelineLayoutCreateInfo:
    // - setLayoutCount is the number of descriptor sets included in the
    //   pipeline layout.
    // - pSetLayouts is a pointer to an array of VkDescriptorSetLayout
    //   objects.
    // - pushConstantRangeCount is the number of push constant ranges
    //   included in the pipeline layout.
    // - pPushConstantRanges is a pointer to an array of VkPushConstantRange
    //   structures defining a set of push constant ranges for use in a
    //   single pipeline layout. In addition to descriptor set layouts, a
    //   pipeline layout also describes how many push constants can be
    //   accessed by each stage of the pipeline.
    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    createInfo.pSetLayouts = descriptorSetLayouts.empty() ? nullptr : descriptorSetLayouts.data();
    createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    createInfo.pPushConstantRanges = 
        pushConstantRanges.empty() ? nullptr : pushConstantRanges.data();

    vkChecker(vkCreatePipelineLayout(LogicalDevice::vkDevice(),
                                     &createInfo,
                                     nullptr,
                                     &mPipelineLayout));
    assert(mPipelineLayout != VK_NULL_HANDLE);
}
}