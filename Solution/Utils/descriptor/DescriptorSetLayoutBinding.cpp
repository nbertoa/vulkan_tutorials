#include "DescriptorSetLayoutBinding.h"

namespace vk2 {
DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(const uint32_t bindingNumber,
                                                       const VkDescriptorType descriptorType,
                                                       const uint32_t descriptorCount,
                                                       const VkShaderStageFlags stageFlags,
                                                       const std::vector<VkSampler>& immutableSamplers) {
    mDescriptorSetLayoutBinding.binding = bindingNumber;
    mDescriptorSetLayoutBinding.descriptorType = descriptorType;
    mDescriptorSetLayoutBinding.descriptorCount = descriptorCount;
    mDescriptorSetLayoutBinding.stageFlags = stageFlags;
    mDescriptorSetLayoutBinding.pImmutableSamplers = 
        immutableSamplers.empty() ? nullptr : immutableSamplers.data();
}
}