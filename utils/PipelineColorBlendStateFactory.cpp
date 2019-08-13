#include "PipelineColorBlendStateFactory.h"

namespace vk {
VkPipelineColorBlendAttachmentState
PipelineColorBlendStateFactory::disableBlending() {
    VkPipelineColorBlendAttachmentState state = {};
    state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                           VK_COLOR_COMPONENT_G_BIT |
                           VK_COLOR_COMPONENT_B_BIT |
                           VK_COLOR_COMPONENT_A_BIT;
    state.blendEnable = VK_FALSE;

    return state;
}

VkPipelineColorBlendAttachmentState
PipelineColorBlendStateFactory::enableBlending() {
    VkPipelineColorBlendAttachmentState state = {};
    state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                           VK_COLOR_COMPONENT_G_BIT |
                           VK_COLOR_COMPONENT_B_BIT |
                           VK_COLOR_COMPONENT_A_BIT;
    state.blendEnable = VK_TRUE;
    state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    state.colorBlendOp = VK_BLEND_OP_ADD;
    state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    state.alphaBlendOp = VK_BLEND_OP_ADD;

    return state;
}

VkPipelineColorBlendStateCreateInfo 
PipelineColorBlendStateFactory::pipelineColorBlendStateCreateInfo(const VkPipelineColorBlendAttachmentState& pipelineColorBlendAttachmentState) {
    VkPipelineColorBlendStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &pipelineColorBlendAttachmentState;

    return createInfo;
}

VkPipelineColorBlendStateCreateInfo
PipelineColorBlendStateFactory::pipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& pipelineColorBlendAttachmentStates) {
    VkPipelineColorBlendStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.attachmentCount = static_cast<uint32_t>(pipelineColorBlendAttachmentStates.size());
    createInfo.pAttachments = pipelineColorBlendAttachmentStates.data();

    return createInfo;
}
}