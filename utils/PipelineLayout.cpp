#include "PipelineLayout.h"

#include <cassert>

#include "DebugUtils.h"
#include "DescriptorSetLayout.h"
#include "LogicalDevice.h"

namespace vk {
PipelineLayout::PipelineLayout(const LogicalDevice& logicalDevice,
                               const DescriptorSetLayout* descriptorSetLayout,
                               const VkPushConstantRange* pushConstantRange)
    : mLogicalDevice(logicalDevice)
{
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

PipelineLayout::PipelineLayout(const LogicalDevice& logicalDevice,
                               const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                               const std::vector<VkPushConstantRange>& pushConstantRanges)
    : mLogicalDevice(logicalDevice)
{
    createPipelineLayout(descriptorSetLayouts,
                         pushConstantRanges);
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(mLogicalDevice.vkDevice(),
                            mPipelineLayout,
                            nullptr);
}

PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mPipelineLayout(other.mPipelineLayout)
{
    other.mPipelineLayout = VK_NULL_HANDLE;
}

const VkPipelineLayout& 
PipelineLayout::pipelineLayout() const {
    assert(mPipelineLayout != VK_NULL_HANDLE);
    return mPipelineLayout;
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