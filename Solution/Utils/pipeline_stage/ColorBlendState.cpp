#include "ColorBlendState.h"

namespace vk2 {
ColorBlendState::ColorBlendState(const ColorBlendAttachmentState& colorBlendAttachmentState,
                                 const vk::Bool32 logicOpEnable,
                                 const vk::LogicOp logicalOperation) 
    : mColorBlendAttachmentState(colorBlendAttachmentState)
{
    mCreateInfo.attachmentCount = 1;
    mCreateInfo.pAttachments = &mColorBlendAttachmentState.state();
    mCreateInfo.logicOpEnable = logicOpEnable;
    mCreateInfo.logicOp = logicalOperation;
}

const vk::PipelineColorBlendStateCreateInfo&
ColorBlendState::state() const {
    return mCreateInfo;
}

ColorBlendState::ColorBlendState(const ColorBlendState& state) 
    : mCreateInfo(state.mCreateInfo)
    , mColorBlendAttachmentState(state.mColorBlendAttachmentState)
{
    mCreateInfo.pAttachments = &mColorBlendAttachmentState.state();
}

const ColorBlendState& 
ColorBlendState::operator=(const ColorBlendState& state) {
    if (this == &state) {
        return *this;
    }

    mCreateInfo = state.mCreateInfo;
    mColorBlendAttachmentState = state.mColorBlendAttachmentState;
    mCreateInfo.pAttachments = &mColorBlendAttachmentState.state();

    return *this;
}
}