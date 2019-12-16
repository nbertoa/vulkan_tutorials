#include "TessellationState.h"

namespace vulkan {
TessellationState::TessellationState(const uint32_t patchControlPoints) {
    mCreateInfo.patchControlPoints = patchControlPoints;
}

const vk::PipelineTessellationStateCreateInfo&
TessellationState::state() const {
    return mCreateInfo;
}
}