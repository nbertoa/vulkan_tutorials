#include "ViewportState.h"

namespace vk {
ViewportState::ViewportState(const VkViewport& viewport,
                             const VkRect2D& scissorRectangle) {
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    mCreateInfo.viewportCount = 1;
    mCreateInfo.pViewports = &viewport;
    mCreateInfo.scissorCount = 1;
    mCreateInfo.pScissors = &scissorRectangle;
}

const VkPipelineViewportStateCreateInfo&
ViewportState::vkState() const {
    return mCreateInfo;
}
}