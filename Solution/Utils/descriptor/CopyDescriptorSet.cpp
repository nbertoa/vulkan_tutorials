#include "CopyDescriptorSet.h"

#include <cassert>

namespace vk2 {
CopyDescriptorSet::CopyDescriptorSet(const VkDescriptorSet sourceDescriptorSet,
                                     const uint32_t sourceBinding,
                                     const uint32_t sourceArrayElement,
                                     const VkDescriptorSet destDescriptorSet,
                                     const uint32_t destBinding,
                                     const uint32_t destArrayElement,
                                     const uint32_t descriptorCount) {
    mDescriptorSet = {};
    mDescriptorSet.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
    mDescriptorSet.srcSet = sourceDescriptorSet;
    mDescriptorSet.srcBinding = sourceBinding;
    mDescriptorSet.srcArrayElement = sourceArrayElement;
    mDescriptorSet.dstSet = destDescriptorSet;
    mDescriptorSet.dstBinding = destBinding;
    mDescriptorSet.dstArrayElement = destArrayElement;
    mDescriptorSet.descriptorCount = descriptorCount;
}

const VkCopyDescriptorSet&
CopyDescriptorSet::vkDescriptorSet() const {
    return mDescriptorSet;
}

VkCopyDescriptorSet&
CopyDescriptorSet::refVkDescriptorSet() {
    return mDescriptorSet;
}
}