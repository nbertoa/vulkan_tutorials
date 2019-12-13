#include "ColorBlendAttachmentState.h"

namespace vulkan {
ColorBlendAttachmentState::ColorBlendAttachmentState(const vk::Bool32 blendEnable,
                                                     const vk::ColorComponentFlags colorWriteMask,
                                                     const vk::BlendFactor srcColorBlendFactor,
                                                     const vk::BlendFactor dstColorBlendFactor,
                                                     const vk::BlendOp colorBlendOp,
                                                     const vk::BlendFactor srcAlphaBlendFactor,
                                                     const vk::BlendFactor dstAlphaBlendFactor,
                                                     const vk::BlendOp alphaBlendOp) {
    mCreateInfo.blendEnable = blendEnable;
    mCreateInfo.colorWriteMask = colorWriteMask;
    mCreateInfo.srcColorBlendFactor = srcColorBlendFactor;
    mCreateInfo.dstColorBlendFactor = dstColorBlendFactor;
    mCreateInfo.colorBlendOp = colorBlendOp;
    mCreateInfo.srcAlphaBlendFactor = srcAlphaBlendFactor;
    mCreateInfo.dstAlphaBlendFactor = dstAlphaBlendFactor;
    mCreateInfo.alphaBlendOp = alphaBlendOp;
}

const vk::PipelineColorBlendAttachmentState&
ColorBlendAttachmentState::state() const {
    return mCreateInfo;
}

void
ColorBlendAttachmentState::disableColorBlending() {
    mCreateInfo.blendEnable = VK_FALSE;
    mCreateInfo.colorWriteMask =
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;
}

void
ColorBlendAttachmentState::enableColorBlending() {
    mCreateInfo.colorWriteMask = 
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;
    mCreateInfo.blendEnable = VK_TRUE;
    mCreateInfo.srcColorBlendFactor = vk::BlendFactor::eOne;
    mCreateInfo.dstColorBlendFactor = vk::BlendFactor::eZero;
    mCreateInfo.colorBlendOp = vk::BlendOp::eAdd;
    mCreateInfo.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    mCreateInfo.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    mCreateInfo.alphaBlendOp = vk::BlendOp::eAdd;
}
}