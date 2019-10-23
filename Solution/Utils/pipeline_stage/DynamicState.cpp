#include "DynamicState.h"

#include <cassert>

namespace vk {
DynamicState::DynamicState(const std::vector<VkDynamicState>& dynamicStates) {
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    mCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    mCreateInfo.pDynamicStates = dynamicStates.empty() ? nullptr : dynamicStates.data();
}

const VkPipelineDynamicStateCreateInfo&
DynamicState::vkState() const {
    return mCreateInfo;
}
}