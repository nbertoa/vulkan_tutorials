#include "DescriptorSets.h"

#include <cassert>

#include "DebugUtils.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "LogicalDevice.h"

namespace vk {
DescriptorSets::DescriptorSets(const LogicalDevice& logicalDevice,
                               const DescriptorPool& descriptorPool,
                               const DescriptorSetLayout& descriptorSetLayout)
    : mLogicalDevice(logicalDevice) {

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    descriptorSetLayouts.push_back(descriptorSetLayout.vkDescriptorSetLayout());
    createDescriptorSets(descriptorPool,
                         descriptorSetLayouts);
}

DescriptorSets::DescriptorSets(const LogicalDevice& logicalDevice,
                               const DescriptorPool& descriptorPool,
                               const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts)
    : mLogicalDevice(logicalDevice) 
{
    createDescriptorSets(descriptorPool,
                        descriptorSetLayouts);
}

DescriptorSets::DescriptorSets(DescriptorSets&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mDescriptorSets(std::move(other.mDescriptorSets)) 
{
    other.mDescriptorSets.clear();
}

void
DescriptorSets::createDescriptorSets(const DescriptorPool& descriptorPool,
                                     const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts) {
    assert(mDescriptorSets.empty());
    assert(descriptorSetLayouts.empty() == false);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool.vkDescriptorPool();
    allocInfo.pSetLayouts = descriptorSetLayouts.data();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());

    mDescriptorSets.resize(descriptorSetLayouts.size());
    vkChecker(vkAllocateDescriptorSets(mLogicalDevice.vkDevice(),
                                       &allocInfo,
                                       mDescriptorSets.data()));
}

void 
DescriptorSets::updateDescriptorSet(const VkWriteDescriptorSet& writeDescriptorSet) {
    assert(mDescriptorSets.empty() == false);
    assert(std::find(mDescriptorSets.begin(), 
                     mDescriptorSets.end(),
                     writeDescriptorSet.dstSet) != mDescriptorSets.end());

    vkUpdateDescriptorSets(mLogicalDevice.vkDevice(),
                           1, // descriptor set count
                           &writeDescriptorSet,
                           0, // descriptor copy count
                           nullptr);
}

void 
DescriptorSets::updateDescriptorSet(const std::vector<VkWriteDescriptorSet>& writeDescriptorSets) {
    assert(mDescriptorSets.empty() == false);
    assert(writeDescriptorSets.empty() == false);
#ifndef NDEBUG // Debug
    for (const VkWriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         writeDescriptorSet.dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(mLogicalDevice.vkDevice(),
                           static_cast<uint32_t>(writeDescriptorSets.size()), // descriptor set count
                           writeDescriptorSets.data(),
                           0, // descriptor copy count
                           nullptr);
}

void
DescriptorSets::updateDescriptorSet(const VkCopyDescriptorSet& copyDescriptorSet) {
    assert(mDescriptorSets.empty() == false);
    assert(std::find(mDescriptorSets.begin(),
                     mDescriptorSets.end(),
                     copyDescriptorSet.srcSet) != mDescriptorSets.end());

    vkUpdateDescriptorSets(mLogicalDevice.vkDevice(),
                           0, // descriptor set count
                           nullptr,
                           1, // descriptor copy count
                           &copyDescriptorSet);
}

void
DescriptorSets::updateDescriptorSet(const std::vector<VkCopyDescriptorSet>& copyDescriptorSets) {
    assert(mDescriptorSets.empty() == false);
    assert(copyDescriptorSets.empty() == false);
#ifndef NDEBUG // Debug
    for (const VkCopyDescriptorSet& copyDescriptorSet : copyDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         copyDescriptorSet.dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(mLogicalDevice.vkDevice(),
                           0, // descriptor set count
                           nullptr,
                           static_cast<uint32_t>(copyDescriptorSets.size()), // descriptor copy count
                           copyDescriptorSets.data());
}

void 
DescriptorSets::updateDescriptorSet(const std::vector<VkWriteDescriptorSet>& writeDescriptorSets,
                                    const std::vector<VkCopyDescriptorSet>& copyDescriptorSets) {
    assert(mDescriptorSets.empty() == false);
    assert(writeDescriptorSets.empty() == false);
    assert(copyDescriptorSets.empty() == false);

#ifndef NDEBUG // Debug
    for (const VkWriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         writeDescriptorSet.dstSet) != mDescriptorSets.end());
    }

    for (const VkCopyDescriptorSet& copyDescriptorSet : copyDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         copyDescriptorSet.dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(mLogicalDevice.vkDevice(),
                           static_cast<uint32_t>(writeDescriptorSets.size()), // descriptor set count
                           writeDescriptorSets.data(),
                           static_cast<uint32_t>(copyDescriptorSets.size()), // descriptor copy count
                           copyDescriptorSets.data());
}

VkDescriptorSet
DescriptorSets::vkDescriptorSet(const uint32_t descriptorSetIndex) const {
    assert(descriptorSetIndex < mDescriptorSets.size());
    return mDescriptorSets[descriptorSetIndex];
}

}