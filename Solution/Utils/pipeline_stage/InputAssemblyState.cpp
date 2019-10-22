#include "InputAssemblyState.h"

namespace vk {
InputAssemblyState::InputAssemblyState(const VkPrimitiveTopology primitiveTopology,
                                       const VkBool32 primitiveRestartEnable) {
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    mCreateInfo.topology = primitiveTopology;
    mCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
}

const VkPipelineInputAssemblyStateCreateInfo&
InputAssemblyState::vkState() const {
    return mCreateInfo;
}
}