#include "MultisampleState.h"

namespace vk {
MultisampleState::MultisampleState(const VkSampleCountFlagBits rasterizationSamples,
                                   const VkBool32 sampleShadingEnable,
                                   const float minSampleShading,
                                   const VkSampleMask* sampleMask,
                                   const VkBool32 alphaToCoverageEnable,
                                   const VkBool32 alphaToOneEnable) {
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    mCreateInfo.rasterizationSamples = rasterizationSamples;
    mCreateInfo.sampleShadingEnable = sampleShadingEnable;
    mCreateInfo.minSampleShading = minSampleShading;
    mCreateInfo.pSampleMask = sampleMask;
    mCreateInfo.alphaToCoverageEnable = alphaToCoverageEnable;
    mCreateInfo.alphaToOneEnable = alphaToOneEnable;
}

const VkPipelineMultisampleStateCreateInfo&
MultisampleState::vkState() const {
    return mCreateInfo;
}
}