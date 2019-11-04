#include "DescriptorSets.h"

#include <cassert>

#include "DebugUtils.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "LogicalDevice.h"
#include "WriteDescriptorSet.h"

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
DescriptorSets::updateDescriptorSet(const WriteDescriptorSet& writeDescriptorSet) {
    assert(mDescriptorSets.empty() == false);
    assert(std::find(mDescriptorSets.begin(), 
                     mDescriptorSets.end(),
                     writeDescriptorSet.vkDescriptorSet().dstSet) != mDescriptorSets.end());

    vkUpdateDescriptorSets(mLogicalDevice.vkDevice(),
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
#ifndef NDEBUG // Debug
    for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         writeDescriptorSet.vkDescriptorSet().dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(mLogicalDevice.vkDevice(),
                           static_cast<uint32_t>(writeDescriptorSets.size()), // descriptor set count
                           reinterpret_cast<const VkWriteDescriptorSet*>(writeDescriptorSets.data()),
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
DescriptorSets::updateDescriptorSet(const std::vector<WriteDescriptorSet>& writeDescriptorSets,
                                    const std::vector<VkCopyDescriptorSet>& copyDescriptorSets) {
    assert(mDescriptorSets.empty() == false);
    assert(writeDescriptorSets.empty() == false);
    assert(copyDescriptorSets.empty() == false);

#ifndef NDEBUG // Debug
    for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         writeDescriptorSet.vkDescriptorSet().dstSet) != mDescriptorSets.end());
    }

    for (const VkCopyDescriptorSet& copyDescriptorSet : copyDescriptorSets) {
        assert(std::find(mDescriptorSets.begin(),
                         mDescriptorSets.end(),
                         copyDescriptorSet.dstSet) != mDescriptorSets.end());
    }
#endif

    vkUpdateDescriptorSets(mLogicalDevice.vkDevice(),
                           static_cast<uint32_t>(writeDescriptorSets.size()), // descriptor set count
                           reinterpret_cast<const VkWriteDescriptorSet*>(writeDescriptorSets.data()),
                           static_cast<uint32_t>(copyDescriptorSets.size()), // descriptor copy count
                           copyDescriptorSets.data());
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