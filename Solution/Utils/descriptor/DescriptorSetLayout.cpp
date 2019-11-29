#include "DescriptorSetLayout.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk2 {
DescriptorSetLayout::DescriptorSetLayout(const std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings) {
    assert(descriptorSetLayoutBindings.empty() == false);

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    createInfo.pBindings = reinterpret_cast<const VkDescriptorSetLayoutBinding*>(descriptorSetLayoutBindings.data());

    vkChecker(vkCreateDescriptorSetLayout(LogicalDevice::vkDevice(),
                                          &createInfo,
                                          nullptr,
                                          &mDescriptorSetLayout));
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(LogicalDevice::vkDevice(),
                                 mDescriptorSetLayout,
                                 nullptr);
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept 
    : mDescriptorSetLayout(other.mDescriptorSetLayout)
{
    other.mDescriptorSetLayout = VK_NULL_HANDLE;
}

VkDescriptorSetLayout
DescriptorSetLayout::vkDescriptorSetLayout() const {
    assert(mDescriptorSetLayout != VK_NULL_HANDLE);

    return mDescriptorSetLayout;
}

}