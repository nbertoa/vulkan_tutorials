#include "ColorBlendState.h"

#include "ColorBlendAttachmentState.h"

namespace vk {
ColorBlendState::ColorBlendState(const ColorBlendAttachmentState& colorBlendAttachmentState,
                                 const VkBool32 logicOpEnable,
                                 const VkLogicOp logicalOperation) {
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    mCreateInfo.attachmentCount = 1;
    mCreateInfo.pAttachments = &colorBlendAttachmentState.vkState();
    mCreateInfo.logicOpEnable = logicOpEnable;
    mCreateInfo.logicOp = logicalOperation;
}

const VkPipelineColorBlendStateCreateInfo&
ColorBlendState::vkState() const {
    return mCreateInfo;
}
}