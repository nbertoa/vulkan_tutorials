#include "WriteDescriptorSet.h"

#include <cassert>

namespace vk {
WriteDescriptorSet::WriteDescriptorSet(const std::vector<VkDescriptorImageInfo>& imageInfos,
                                       const VkDescriptorSet destDescriptorSet,
                                       const VkDescriptorType descriptorType,
                                       const uint32_t destBinding,
                                       const uint32_t destArrayStartIndex) {
    assert(imageInfos.empty() == false);
    mDescriptorSet = {};
    mDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    mDescriptorSet.dstSet = destDescriptorSet;
    mDescriptorSet.descriptorType = descriptorType;
    mDescriptorSet.dstBinding = destBinding;
    mDescriptorSet.dstArrayElement = destArrayStartIndex;
    mDescriptorSet.pImageInfo = imageInfos.data();
    mDescriptorSet.descriptorCount = static_cast<uint32_t>(imageInfos.size());

}

WriteDescriptorSet::WriteDescriptorSet(const std::vector<VkDescriptorBufferInfo>& bufferInfos,
                                       const VkDescriptorSet destDescriptorSet,
                                       const VkDescriptorType descriptorType,
                                       const uint32_t destBinding,
                                       const uint32_t destArrayStartIndex) {
    assert(bufferInfos.empty() == false);
    mDescriptorSet = {};
    mDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    mDescriptorSet.dstSet = destDescriptorSet;
    mDescriptorSet.descriptorType = descriptorType;
    mDescriptorSet.dstBinding = destBinding;
    mDescriptorSet.dstArrayElement = destArrayStartIndex;
    mDescriptorSet.pBufferInfo = bufferInfos.data();
    mDescriptorSet.descriptorCount = static_cast<uint32_t>(bufferInfos.size());
}

WriteDescriptorSet::WriteDescriptorSet(const std::vector<VkBufferView>& texelBufferViews,
                                       const VkDescriptorSet destDescriptorSet,
                                       const VkDescriptorType descriptorType,
                                       const uint32_t destBinding,
                                       const uint32_t destArrayStartIndex) {
    assert(texelBufferViews.empty() == false);
    mDescriptorSet = {};
    mDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    mDescriptorSet.dstSet = destDescriptorSet;
    mDescriptorSet.dstBinding = destBinding;
    mDescriptorSet.dstArrayElement = destArrayStartIndex;
    mDescriptorSet.pTexelBufferView = texelBufferViews.data();
    mDescriptorSet.descriptorCount = static_cast<uint32_t>(texelBufferViews.size());
}

const VkWriteDescriptorSet&
WriteDescriptorSet::vkDescriptorSet() const {
    return mDescriptorSet;
}

VkWriteDescriptorSet&
WriteDescriptorSet::refVkDescriptorSet() {
    return mDescriptorSet;
}
}