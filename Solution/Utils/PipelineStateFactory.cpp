#include "PipelineStateFactory.h"

namespace vk {
void
PipelineStateFactory::defaultDepthStencilState(VkPipelineDepthStencilStateCreateInfo& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    createInfo.depthTestEnable = VK_TRUE;
    createInfo.depthWriteEnable = VK_TRUE;
    createInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    createInfo.depthBoundsTestEnable = VK_FALSE;
    createInfo.minDepthBounds = 0.0f;
    createInfo.maxDepthBounds = 1.0f;
    createInfo.stencilTestEnable = VK_FALSE;
    createInfo.front = {};
    createInfo.back = {};
}

void
PipelineStateFactory::colorBlendState(const VkPipelineColorBlendAttachmentState& colorBlendAttachmentState,
                                      VkPipelineColorBlendStateCreateInfo& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorBlendAttachmentState;
}

void
PipelineStateFactory::colorBlendState(const std::vector<VkPipelineColorBlendAttachmentState>& colorBlendAttachmentStates,
                                      VkPipelineColorBlendStateCreateInfo& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
    createInfo.pAttachments = colorBlendAttachmentStates.data();
}
}