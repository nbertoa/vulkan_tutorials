#include "ColorBlendAttachmentState.h"

namespace vk {
ColorBlendAttachmentState::ColorBlendAttachmentState(const VkBool32 blendEnable,
                                                     const VkColorComponentFlags colorWriteMask,
                                                     const VkBlendFactor srcColorBlendFactor,
                                                     const VkBlendFactor dstColorBlendFactor,
                                                     const VkBlendOp colorBlendOp,
                                                     const VkBlendFactor srcAlphaBlendFactor,
                                                     const VkBlendFactor dstAlphaBlendFactor,
                                                     const VkBlendOp alphaBlendOp) {
    mCreateInfo.blendEnable = blendEnable;
    mCreateInfo.colorWriteMask = colorWriteMask;
    mCreateInfo.srcColorBlendFactor = srcColorBlendFactor;
    mCreateInfo.dstColorBlendFactor = dstColorBlendFactor;
    mCreateInfo.colorBlendOp = colorBlendOp;
    mCreateInfo.srcAlphaBlendFactor = srcAlphaBlendFactor;
    mCreateInfo.dstAlphaBlendFactor = dstAlphaBlendFactor;
    mCreateInfo.alphaBlendOp = alphaBlendOp;
}

const VkPipelineColorBlendAttachmentState&
ColorBlendColorBlendAttachmentStateAttachmentState::vkState() const {
    return mCreateInfo;
}
}