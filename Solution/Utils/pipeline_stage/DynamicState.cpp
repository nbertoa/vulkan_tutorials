#include "DynamicState.h"

#include <cassert>

namespace vk {
DynamicState::DynamicState(const std::vector<VkDynamicState>& dynamicStates) 
    : mDynamicStates(dynamicStates)
{
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    mCreateInfo.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
    mCreateInfo.pDynamicStates = mDynamicStates.empty() ? nullptr : mDynamicStates.data();
}

const VkPipelineDynamicStateCreateInfo&
DynamicState::vkState() const {
    return mCreateInfo;
}

DynamicState::DynamicState(const DynamicState& state)
    : mCreateInfo(state.mCreateInfo)
    , mDynamicStates(state.mDynamicStates) 
{
    mCreateInfo.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
    mCreateInfo.pDynamicStates = mDynamicStates.empty() ? nullptr : mDynamicStates.data();
}
const DynamicState& DynamicState::operator=(const DynamicState& state) {
    if (this == &state) {
        return *this;
    }

    mCreateInfo = state.mCreateInfo;
    mDynamicStates = state.mDynamicStates;
    mCreateInfo.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
    mCreateInfo.pDynamicStates = mDynamicStates.empty() ? nullptr : mDynamicStates.data();
}
}