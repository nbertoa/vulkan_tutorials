#include "MultisampleState.h"

namespace vulkan {
MultisampleState::MultisampleState(const vk::SampleCountFlagBits rasterizationSamples,
                                   const vk::Bool32 sampleShadingEnable,
                                   const float minSampleShading,
                                   const vk::SampleMask* sampleMask,
                                   const vk::Bool32 alphaToCoverageEnable,
                                   const vk::Bool32 alphaToOneEnable) {
    mCreateInfo.rasterizationSamples = rasterizationSamples;
    mCreateInfo.sampleShadingEnable = sampleShadingEnable;
    mCreateInfo.minSampleShading = minSampleShading;
    mCreateInfo.pSampleMask = sampleMask;
    mCreateInfo.alphaToCoverageEnable = alphaToCoverageEnable;
    mCreateInfo.alphaToOneEnable = alphaToOneEnable;
}

const vk::PipelineMultisampleStateCreateInfo&
MultisampleState::state() const {
    return mCreateInfo;
}
}