#include "TessellationState.h"

namespace vk2 {
TessellationState::TessellationState(const uint32_t patchControlPoints) {
    mCreateInfo.patchControlPoints = patchControlPoints;
}

const vk::PipelineTessellationStateCreateInfo&
TessellationState::state() const {
    return mCreateInfo;
}
}