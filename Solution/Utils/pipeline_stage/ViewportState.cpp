#include "ViewportState.h"

namespace vk {
ViewportState::ViewportState(const VkViewport& viewport,
                             const VkRect2D& scissorRectangle) 
    : mViewport(viewport)
    , mScissorRectangle(scissorRectangle)
{
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    mCreateInfo.viewportCount = 1;
    mCreateInfo.pViewports = &mViewport;
    mCreateInfo.scissorCount = 1;
    mCreateInfo.pScissors = &mScissorRectangle;
}

const VkPipelineViewportStateCreateInfo&
ViewportState::vkState() const {
    return mCreateInfo;
}

ViewportState::ViewportState(const ViewportState& state)
    : mCreateInfo(state.mCreateInfo)
    , mViewport(state.mViewport) 
    , mScissorRectangle(state.mScissorRectangle)
{
}

const ViewportState& ViewportState::operator=(const ViewportState& state) {
    if (this == &state) {
        return *this;
    }

    mCreateInfo = state.mCreateInfo;
    mViewport = state.mViewport;
    mScissorRectangle = state.mScissorRectangle;
    mCreateInfo.pViewports = &mViewport;
    mCreateInfo.pScissors = &mScissorRectangle;
}
}