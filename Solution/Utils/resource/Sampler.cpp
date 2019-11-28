#include "Sampler.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk {
Sampler::Sampler(const VkFilter magnificationFilter,
                 const VkFilter minificationFilter,
                 const VkSamplerMipmapMode mipmapFilter,
                 const VkSamplerAddressMode addressModeU,
                 const VkSamplerAddressMode addressModeV,
                 const VkSamplerAddressMode addressModeW,
                 const float mipmapLodBias,
                 const VkBool32 anisotropyEnable,
                 const float maxAnisotropy,
                 const VkBool32 compareEnable,
                 const VkCompareOp compareOperator,
                 const float minLod,
                 const float maxLod,
                 const VkBorderColor predefinedBorderColor,
                 const VkBool32 unnormalizedCoordinates) {
    VkSamplerCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = magnificationFilter;
    createInfo.minFilter = minificationFilter;
    createInfo.mipmapMode = mipmapFilter;
    createInfo.addressModeU = addressModeU;
    createInfo.addressModeV = addressModeV;
    createInfo.addressModeW = addressModeW;
    createInfo.mipLodBias = mipmapLodBias;
    createInfo.anisotropyEnable = anisotropyEnable;
    createInfo.maxAnisotropy = maxAnisotropy;
    createInfo.compareEnable = compareEnable;
    createInfo.compareOp = compareOperator;
    createInfo.minLod = minLod;
    createInfo.maxLod = maxLod;
    createInfo.borderColor = predefinedBorderColor;
    createInfo.unnormalizedCoordinates = unnormalizedCoordinates;

    vkChecker(vkCreateSampler(LogicalDevice::vkDevice(),
                              &createInfo,
                              nullptr,
                              &mSampler));
}

Sampler::~Sampler() {
    vkDestroySampler(LogicalDevice::vkDevice(),
                     mSampler,
                     nullptr);
}

Sampler::Sampler(Sampler&& other) noexcept
    : mSampler(other.mSampler)
{
    other.mSampler = VK_NULL_HANDLE;
}

VkSampler
Sampler::vkSampler() const {
    assert(mSampler != VK_NULL_HANDLE);
    return mSampler;
}
}