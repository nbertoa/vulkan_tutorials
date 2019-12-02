#ifndef UTILS_PIPELINE_STAGE_MULTISAMPLE_STATE
#define UTILS_PIPELINE_STAGE_MULTISAMPLE_STATE

#include <vulkan/vulkan.hpp>

namespace vk2 {
//
// VkPipelineMultisampleStateCreateInfo wrapper
//
// The VkPipelineMultisampleStateCreateInfo struct configures multisampling, 
// which is one of the ways to perform anti-aliasing.
//
// It works by combining the fragment shader results of multiple polygons that 
// rasterize to the same pixel.
//
// This mainly occurs along edges, which is also where 
// the most noticeable aliasing artifacts occur.
//
// Because it does not need to run the fragment shader multiple times if only 
// one polygon maps to a pixel, it is significantly less expensive than simply rendering to a 
// higher resolution and then downscaling. 
//
// Enabling it requires enabling a GPU feature.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class MultisampleState {
public:
    // * rasterizationSamples specifies the number of samples per 
    //   pixel used in rasterization (VK_SAMPLE_COUNT_):
    //
    //   . 1_BIT, 2_BIT, 4_BIT, 8_BIT, 16_BIT, 32_BIT, 64_BIT
    //
    // * sampleShadingEnable
    //
    // * minSampleShading fraction if sampleShadingEnable is set to VK_TRUE.
    //
    // * sampleMask of static coverage information that is ANDed with the coverage information 
    //   generated during rasterization.
    //
    // * alphaToCoverageEnable controls whether a temporary coverage value is 
    //   generated based on the alpha component of the fragment’s first color output.
    //
    // * alphaToOneEnable controls whether the alpha component of the fragment’s first color 
    //   output is replaced with one.
    //
    // The default constructor values disable multisampling.
    MultisampleState(const VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                     const VkBool32 sampleShadingEnable = VK_FALSE,
                     const float minSampleShading = 1.0f,
                     const VkSampleMask* sampleMask = nullptr,
                     const VkBool32 alphaToCoverageEnable = VK_FALSE,
                     const VkBool32 alphaToOneEnable = VK_FALSE);

    const VkPipelineMultisampleStateCreateInfo&
    vkState() const;

private:
    VkPipelineMultisampleStateCreateInfo mCreateInfo = {};
};
}

#endif 