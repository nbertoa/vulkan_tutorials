#include "DepthStencilState.h"

namespace vulkan {
DepthStencilState::DepthStencilState(const vk::Bool32 depthTestEnable,
                                     const vk::Bool32 depthWriteEnable,
                                     const vk::CompareOp depthCompareOp,
                                     const vk::Bool32 depthBoundsTestEnable,
                                     const vk::Bool32 stencilTestEnable,
                                     const vk::StencilOpState front,
                                     const vk::StencilOpState back,
                                     const float minDepthBounds,
                                     const float maxDepthBounds) {
    mCreateInfo.depthTestEnable = depthTestEnable;
    mCreateInfo.depthWriteEnable = depthWriteEnable;
    mCreateInfo.depthCompareOp = depthCompareOp;
    mCreateInfo.depthBoundsTestEnable = depthBoundsTestEnable;
    mCreateInfo.stencilTestEnable = stencilTestEnable;
    mCreateInfo.front = front;
    mCreateInfo.back = back;
    mCreateInfo.minDepthBounds = minDepthBounds;
    mCreateInfo.maxDepthBounds = maxDepthBounds;
}

const vk::PipelineDepthStencilStateCreateInfo&
DepthStencilState::state() const {
    return mCreateInfo;
}

void
DepthStencilState::enableDepthDisableStencil() {
    mCreateInfo.depthTestEnable = VK_TRUE;
    mCreateInfo.depthWriteEnable = VK_TRUE;
    mCreateInfo.depthCompareOp = vk::CompareOp::eLess;
    mCreateInfo.depthBoundsTestEnable = VK_FALSE;
    mCreateInfo.stencilTestEnable = VK_FALSE;
    mCreateInfo.front = {};
    mCreateInfo.back = {};
    mCreateInfo.minDepthBounds = 0.0f;
    mCreateInfo.maxDepthBounds = 1.0f;
}
}