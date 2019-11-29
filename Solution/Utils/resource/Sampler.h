#ifndef UTILS_RESOURCE_SAMPLER
#define UTILS_RESOURCE_SAMPLER

#include <vulkan/vulkan.h>

namespace vk2 {
//
// VkSampler wrapper
//
// VkSampler objects represent the state of an image sampler which 
// is used by the implementation to read image dataand apply filtering
// and other transformations for the shader.
//
// You need the Sampler to:
// - Update descriptors sets through vkUpdateDescriptorSets
//
// To create/use the Image you need:
// - DeviceMemory for vkBindImageMemory
//
class Sampler {
public:
    // * magnificationFilter to apply to lookups (VK_FILTER_)    
    // * minificationFilter to apply to lookups (VK_FILTER_)
    //
    //   - NEAREST, LINEAR, CUBIC_IMG
    //
    // * mipmapFilter to apply to lookups (VK_SAMPLER_MIPMAP_MODE_)
    //
    //   - NEAREST, LINEAR
    //
    // * addressModeU for outside [0..1] range for U coordinate (VK_SAMPLER_ADDRESS_MODE_)
    // * addressModeV for outside [0..1] range for V coordinate (VK_SAMPLER_ADDRESS_MODE_)
    // * addressModeW for outside [0..1] range for W coordinate (VK_SAMPLER_ADDRESS_MODE_)
    //
    //   - REPEAT, MIRRORED_REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER, MIRROR_CLAMP_TO_EDGE
    //
    // * mipmapLodBias is the bias to be added to mipmap LOD (level-of-detail) 
    //   calculation and bias provided by image sampling functions in SPIR-V
    //
    // * anisotropyEnable is True to enable anisotropic filtering.
    //
    // * maxAnisotropy is the anisotropy value clamp used by the sampler when
    //   anisotropyEnable is True. Otherwise, it is ignored
    //
    // * compareEnable is True to enable comparison against a reference value
    //   during lookups, or False otherwise.
    //
    // * compareOperator to apply to fetched data before filtering (VK_COMPARE_OP_)
    //
    //   - NEVER, LESS, EQUAL, LESS_OR_EQUAL, GREATER, NOT_EQUAL, 
    //     GREATER_OR_EQUAL, ALWAYS,
    //
    // * minLod and maxLod used to clamp the computed LOD value
    //
    // * predefinedBorderColor to use (VK_BORDER_COLOR_)
    //
    //   - FLOAT_TRANSPARENT_BLACK, INT_TRANSPARENT_BLACK, FLOAT_OPAQUE_BLACK
    //     INT_OPAQUE_BLACK, FLOAT_OPAQUE_WHITE, INT_OPAQUE_WHITE
    //
    // * unnormalizedCoordinates controls wether to use unnormalized
    //   or normalized texel coordinates to address texels of the image.
    //   When se to VK_TRUE, the range of the image coordinates used to
    //   lookup the texel is in the range of zero to the image
    //   dimensions for x, y and z.
    //   When se to VK_FALSE the range of image coordinates is zero to one.
    //
    // Notes: The global logical device creates the sampler.
    Sampler(const VkFilter magnificationFilter = VK_FILTER_LINEAR,
            const VkFilter minificationFilter = VK_FILTER_LINEAR,
            const VkSamplerMipmapMode mipmapFilter = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            const VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            const VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT, 
            const VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            const float mipmapLodBias = 0.0f,
            const VkBool32 anisotropyEnable = VK_TRUE,
            const float maxAnisotropy = 16.0f,
            const VkBool32 compareEnable = VK_TRUE,
            const VkCompareOp compareOperator = VK_COMPARE_OP_ALWAYS,
            const float minLod = 0.0f,
            const float maxLod = 0.0f,
            const VkBorderColor predefinedBorderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            const VkBool32 unnormalizedCoordinates = VK_FALSE);
    ~Sampler();
    Sampler(Sampler&&) noexcept;
    Sampler(const Sampler&) = delete;
    const Sampler& operator=(const Sampler&) = delete;

    VkSampler
    vkSampler() const;

private:
    VkSampler mSampler = VK_NULL_HANDLE;
};
}

#endif 