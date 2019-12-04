#include "DescriptorSets.h"

#include <cassert>

#include "DescriptorSetLayout.h"
#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk2 {

DescriptorSets::DescriptorSets(const vk::DescriptorPool& descriptorPool,
                               const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts)
{
    assert(descriptorSetLayouts.empty() == false);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = (VkDescriptorPool)descriptorPool;
    allocInfo.pSetLayouts = descriptorSetLayouts.data();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());

    mDescriptorSets.resize(descriptorSetLayouts.size());
    vkChecker(vkAllocateDescriptorSets(LogicalDevice::device(),
                                       &allocInfo,
                                       mDescriptorSets.data()));
}

DescriptorSets::DescriptorSets(DescriptorSets&& other) noexcept
    : mDescriptorSets(std::move(other.mDescriptorSets)) 
{
    other.mDescriptorSets.clear();
}

void 
DescriptorSets::updateDescriptorSet(const vk::WriteDescriptorSet& writeDescriptorSet) {
    assert(mDescriptorSets.empty() == false);
    assert(std::find(mDescriptorSets.begin(), 
                     mDescriptorSets.end(),
                     writeDescriptorSet.dstSet) != mDescriptorSets.end());

    vkUpdateDescriptorSets(LogicalDevice::device(),
                           1, // descriptor set count
                           (VkWriteDescriptorSet*)&writeDescriptorSet,
                           0, // descriptor copy count
                           nullptr);
}

void
DescriptorSets::updateDescriptorSet(const uint32_t descriptorSetIndex,
                                    vk::WriteDescriptorSet& writeDescriptorSet) {
    writeDescriptorSet.dstSet = operator[](descriptorSetIndex);
    updateDescriptorSet(writeDescriptorSet);
}

void 
DescriptorSets::updateDescriptorSet(const std::vector<vk::WriteDescriptorSet>& writeDescriptorSets) {
    assert(mDescriptorSets.empty() == false);
    assert(writeDescriptorSets.empty() == false);
#ifdef _DEBUG
    for (const vk::WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         writeDescriptorSet.dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(LogicalDevice::device(),
                           static_cast<uint32_t>(writeDescriptorSets.size()), // desc set count
                           (VkWriteDescriptorSet*)writeDescriptorSets.data(),
                           0, // descriptor copy count
                           nullptr);
}

void
DescriptorSets::updateDescriptorSet(const vk::CopyDescriptorSet& copyDescriptorSet) {
    assert(mDescriptorSets.empty() == false);
    assert(std::find(mDescriptorSets.begin(),
                     mDescriptorSets.end(),
                     copyDescriptorSet.srcSet) != mDescriptorSets.end());

    vkUpdateDescriptorSets(LogicalDevice::device(),
                           0, // descriptor set count
                           nullptr,
                           1, // descriptor copy count
                           (VkCopyDescriptorSet*)&copyDescriptorSet);
}

void
DescriptorSets::updateDescriptorSet(const std::vector<vk::CopyDescriptorSet>& copyDescriptorSets) {
    assert(mDescriptorSets.empty() == false);
    assert(copyDescriptorSets.empty() == false);
#ifdef _DEBUG
    for (const vk::CopyDescriptorSet& copyDescriptorSet : copyDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         copyDescriptorSet.dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(LogicalDevice::device(),
                           0, // descriptor set count
                           nullptr,
                           static_cast<uint32_t>(copyDescriptorSets.size()), // desc copy count
                           (VkCopyDescriptorSet*)copyDescriptorSets.data());
}

void 
DescriptorSets::updateDescriptorSet(const std::vector<vk::WriteDescriptorSet>& writeDescriptorSets,
                                    const std::vector<vk::CopyDescriptorSet>& copyDescriptorSets) {
    assert(mDescriptorSets.empty() == false);
    assert(writeDescriptorSets.empty() == false);
    assert(copyDescriptorSets.empty() == false);

#ifdef _DEBUG
    for (const vk::WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         writeDescriptorSet.dstSet) != mDescriptorSets.end());
    }

    for (const vk::CopyDescriptorSet& copyDescriptorSet : copyDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         copyDescriptorSet.dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(LogicalDevice::device(),
                           static_cast<uint32_t>(writeDescriptorSets.size()), // desc set count
                           (VkWriteDescriptorSet*)writeDescriptorSets.data(),
                           static_cast<uint32_t>(copyDescriptorSets.size()), // desc copy count
                           (VkCopyDescriptorSet*)copyDescriptorSets.data());
}

VkDescriptorSet
DescriptorSets::operator[](const uint32_t index) const {
    assert(index < mDescriptorSets.size());
    return mDescriptorSets[index];
}

uint32_t
DescriptorSets::size() const {
    return static_cast<uint32_t>(mDescriptorSets.size());
}

}