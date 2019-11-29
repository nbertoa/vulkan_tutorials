#include "TessellationState.h"

namespace vk2 {
TessellationState::TessellationState(const uint32_t patchControlPoints) {
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    mCreateInfo.patchControlPoints = patchControlPoints;
}

const VkPipelineTessellationStateCreateInfo&
TessellationState::vkState() const {
    return mCreateInfo;
}
}