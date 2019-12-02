#include "DescriptorPool.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk2 {
DescriptorPool::DescriptorPool(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
                               const uint32_t maxDescriptorSetCount,
                               const VkDescriptorPoolCreateFlags flags) {
    createPool(descriptorPoolSizes,
               maxDescriptorSetCount,
               flags);
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(LogicalDevice::device(),
                            mDescriptorPool,
                            nullptr);
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    : mDescriptorPool(other.mDescriptorPool)
{
    other.mDescriptorPool = VK_NULL_HANDLE;
}

VkDescriptorPool
DescriptorPool::vkDescriptorPool() const {
    assert(mDescriptorPool != VK_NULL_HANDLE);
    return mDescriptorPool;
}

void 
DescriptorPool::createPool(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
                           const uint32_t maxDescriptorSetCount,
                           const VkDescriptorPoolCreateFlags flags) {
    assert(mDescriptorPool == VK_NULL_HANDLE);
    assert(descriptorPoolSizes.empty() == false);

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    createInfo.pPoolSizes = descriptorPoolSizes.data();
    createInfo.maxSets = maxDescriptorSetCount;
    createInfo.flags = flags;

    vkChecker(vkCreateDescriptorPool(LogicalDevice::device(),
                                     &createInfo,
                                     nullptr,
                                     &mDescriptorPool));
}

}