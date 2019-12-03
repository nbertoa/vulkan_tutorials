#include "InputAssemblyState.h"

namespace vk2 {
InputAssemblyState::InputAssemblyState(const vk::PrimitiveTopology primitiveTopology,
                                       const vk::Bool32 primitiveRestartEnable) {
    mCreateInfo.topology = primitiveTopology;
    mCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
}

const vk::PipelineInputAssemblyStateCreateInfo&
InputAssemblyState::state() const {
    return mCreateInfo;
}
}