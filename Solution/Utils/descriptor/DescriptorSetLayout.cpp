#include "DescriptorSetLayout.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk {
DescriptorSetLayout::DescriptorSetLayout(const LogicalDevice& logicalDevice,
                                         const std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings)
    : mLogicalDevice(logicalDevice)
{
    assert(descriptorSetLayoutBindings.empty() == false);

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    createInfo.pBindings = reinterpret_cast<const VkDescriptorSetLayoutBinding*>(descriptorSetLayoutBindings.data());

    vkChecker(vkCreateDescriptorSetLayout(mLogicalDevice.vkDevice(),
                                          &createInfo,
                                          nullptr,
                                          &mDescriptorSetLayout));
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(mLogicalDevice.vkDevice(),
                                 mDescriptorSetLayout,
                                 nullptr);
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept 
    : mLogicalDevice(other.mLogicalDevice)
    , mDescriptorSetLayout(other.mDescriptorSetLayout)
{
    other.mDescriptorSetLayout = VK_NULL_HANDLE;
}

VkDescriptorSetLayout
DescriptorSetLayout::vkDescriptorSetLayout() const {
    assert(mDescriptorSetLayout != VK_NULL_HANDLE);

    return mDescriptorSetLayout;
}

}