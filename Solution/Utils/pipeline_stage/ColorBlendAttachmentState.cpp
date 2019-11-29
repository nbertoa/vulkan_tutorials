#include "ColorBlendAttachmentState.h"

namespace vk2 {
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
ColorBlendAttachmentState::vkState() const {
    return mCreateInfo;
}

void
ColorBlendAttachmentState::disableColorBlending() {
    mCreateInfo.blendEnable = VK_FALSE;
    mCreateInfo.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
}

void
ColorBlendAttachmentState::enableColorBlending() {
    mCreateInfo.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    mCreateInfo.blendEnable = VK_TRUE;
    mCreateInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    mCreateInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    mCreateInfo.colorBlendOp = VK_BLEND_OP_ADD;
    mCreateInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    mCreateInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    mCreateInfo.alphaBlendOp = VK_BLEND_OP_ADD;
}
}