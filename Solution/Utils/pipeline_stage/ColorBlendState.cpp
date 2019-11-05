#include "ColorBlendState.h"

namespace vk {
ColorBlendState::ColorBlendState(const ColorBlendAttachmentState& colorBlendAttachmentState,
                                 const VkBool32 logicOpEnable,
                                 const VkLogicOp logicalOperation) 
    : mColorBlendAttachmentState(colorBlendAttachmentState)
{
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    mCreateInfo.attachmentCount = 1;
    mCreateInfo.pAttachments = &mColorBlendAttachmentState.vkState();
    mCreateInfo.logicOpEnable = logicOpEnable;
    mCreateInfo.logicOp = logicalOperation;
}

const VkPipelineColorBlendStateCreateInfo&
ColorBlendState::vkState() const {
    return mCreateInfo;
}

ColorBlendState::ColorBlendState(const ColorBlendState& state) 
    : mCreateInfo(state.mCreateInfo)
    , mColorBlendAttachmentState(state.mColorBlendAttachmentState)
{
    mCreateInfo.pAttachments = &mColorBlendAttachmentState.vkState();
}

const ColorBlendState& 
ColorBlendState::operator=(const ColorBlendState& state) {
    if (this == &state) {
        return *this;
    }

    mCreateInfo = state.mCreateInfo;
    mColorBlendAttachmentState = state.mColorBlendAttachmentState;
    mCreateInfo.pAttachments = &mColorBlendAttachmentState.vkState();

    return *this;
}
}