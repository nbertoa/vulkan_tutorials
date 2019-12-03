#include "ViewportState.h"

namespace vk2 {
ViewportState::ViewportState(const vk::Viewport& viewport,
                             const vk::Rect2D& scissorRectangle) 
    : mViewport(viewport)
    , mScissorRectangle(scissorRectangle)
{
    mCreateInfo.viewportCount = 1;
    mCreateInfo.pViewports = &mViewport;
    mCreateInfo.scissorCount = 1;
    mCreateInfo.pScissors = &mScissorRectangle;
}

const vk::PipelineViewportStateCreateInfo&
ViewportState::state() const {
    return mCreateInfo;
}

ViewportState::ViewportState(const ViewportState& state)
    : mCreateInfo(state.mCreateInfo)
    , mViewport(state.mViewport) 
    , mScissorRectangle(state.mScissorRectangle)
{
}

const ViewportState& 
ViewportState::operator=(const ViewportState& state) {
    if (this == &state) {
        return *this;
    }

    mCreateInfo = state.mCreateInfo;
    mViewport = state.mViewport;
    mScissorRectangle = state.mScissorRectangle;
    mCreateInfo.pViewports = &mViewport;
    mCreateInfo.pScissors = &mScissorRectangle;

    return *this;
}
}