#include "DepthStencilState.h"

namespace vk {
DepthStencilState::DepthStencilState(const VkBool32 depthTestEnable,
                                     const VkBool32 depthWriteEnable,
                                     const VkCompareOp depthCompareOp,
                                     const VkBool32 depthBoundsTestEnable,
                                     const VkBool32 stencilTestEnable,
                                     const VkStencilOpState front,
                                     const VkStencilOpState back,
                                     const float minDepthBounds,
                                     const float maxDepthBounds) {
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
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

const VkPipelineDepthStencilStateCreateInfo&
DepthStencilState::vkState() const {
    return mCreateInfo;
}

void
DepthStencilState::enableDepthDisableStencil() {
    mCreateInfo.depthTestEnable = VK_TRUE;
    mCreateInfo.depthWriteEnable = VK_TRUE;
    mCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    mCreateInfo.depthBoundsTestEnable = VK_FALSE;
    mCreateInfo.stencilTestEnable = VK_FALSE;
    mCreateInfo.front = {};
    mCreateInfo.back = {};
    mCreateInfo.minDepthBounds = 0.0f;
    mCreateInfo.maxDepthBounds = 1.0f;
}
}