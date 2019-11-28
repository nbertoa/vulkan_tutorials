#include "DescriptorSets.h"

#include <cassert>

#include "CopyDescriptorSet.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "WriteDescriptorSet.h"
#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk {

DescriptorSets::DescriptorSets(const DescriptorPool& descriptorPool,
                               const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts)
{
    assert(descriptorSetLayouts.empty() == false);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool.vkDescriptorPool();
    allocInfo.pSetLayouts = descriptorSetLayouts.data();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());

    mDescriptorSets.resize(descriptorSetLayouts.size());
    vkChecker(vkAllocateDescriptorSets(LogicalDevice::vkDevice(),
                                       &allocInfo,
                                       mDescriptorSets.data()));
}

DescriptorSets::DescriptorSets(DescriptorSets&& other) noexcept
    : mDescriptorSets(std::move(other.mDescriptorSets)) 
{
    other.mDescriptorSets.clear();
}

void 
DescriptorSets::updateDescriptorSet(const WriteDescriptorSet& writeDescriptorSet) {
    assert(mDescriptorSets.empty() == false);
    assert(std::find(mDescriptorSets.begin(), 
                     mDescriptorSets.end(),
                     writeDescriptorSet.vkDescriptorSet().dstSet) != mDescriptorSets.end());

    vkUpdateDescriptorSets(LogicalDevice::vkDevice(),
                           1, // descriptor set count
                           &writeDescriptorSet.vkDescriptorSet(),
                           0, // descriptor copy count
                           nullptr);
}

void
DescriptorSets::updateDescriptorSet(const uint32_t descriptorSetIndex,
                                    WriteDescriptorSet& writeDescriptorSet) {
    writeDescriptorSet.refVkDescriptorSet().dstSet = operator[](descriptorSetIndex);
    updateDescriptorSet(writeDescriptorSet);
}

void 
DescriptorSets::updateDescriptorSet(const std::vector<WriteDescriptorSet>& writeDescriptorSets) {
    assert(mDescriptorSets.empty() == false);
    assert(writeDescriptorSets.empty() == false);
#ifdef _DEBUG
    for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         writeDescriptorSet.vkDescriptorSet().dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(LogicalDevice::vkDevice(),
                           static_cast<uint32_t>(writeDescriptorSets.size()), // desc set count
                           reinterpret_cast<const VkWriteDescriptorSet*>(writeDescriptorSets.data()),
                           0, // descriptor copy count
                           nullptr);
}

void
DescriptorSets::updateDescriptorSet(const CopyDescriptorSet& copyDescriptorSet) {
    assert(mDescriptorSets.empty() == false);
    assert(std::find(mDescriptorSets.begin(),
                     mDescriptorSets.end(),
                     copyDescriptorSet.vkDescriptorSet().srcSet) != mDescriptorSets.end());

    vkUpdateDescriptorSets(LogicalDevice::vkDevice(),
                           0, // descriptor set count
                           nullptr,
                           1, // descriptor copy count
                           &copyDescriptorSet.vkDescriptorSet());
}

void
DescriptorSets::updateDescriptorSet(const std::vector<CopyDescriptorSet>& copyDescriptorSets) {
    assert(mDescriptorSets.empty() == false);
    assert(copyDescriptorSets.empty() == false);
#ifdef _DEBUG
    for (const CopyDescriptorSet& copyDescriptorSet : copyDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         copyDescriptorSet.vkDescriptorSet().dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(LogicalDevice::vkDevice(),
                           0, // descriptor set count
                           nullptr,
                           static_cast<uint32_t>(copyDescriptorSets.size()), // desc copy count
                           reinterpret_cast<const VkCopyDescriptorSet*>(copyDescriptorSets.data()));
}

void 
DescriptorSets::updateDescriptorSet(const std::vector<WriteDescriptorSet>& writeDescriptorSets,
                                    const std::vector<CopyDescriptorSet>& copyDescriptorSets) {
    assert(mDescriptorSets.empty() == false);
    assert(writeDescriptorSets.empty() == false);
    assert(copyDescriptorSets.empty() == false);

#ifdef _DEBUG
    for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         writeDescriptorSet.vkDescriptorSet().dstSet) != mDescriptorSets.end());
    }

    for (const CopyDescriptorSet& copyDescriptorSet : copyDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         copyDescriptorSet.vkDescriptorSet().dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(LogicalDevice::vkDevice(),
                           static_cast<uint32_t>(writeDescriptorSets.size()), // desc set count
                           reinterpret_cast<const VkWriteDescriptorSet*>(writeDescriptorSets.data()),
                           static_cast<uint32_t>(copyDescriptorSets.size()), // desc copy count
                           reinterpret_cast<const VkCopyDescriptorSet*>(copyDescriptorSets.data()));
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