#include "DynamicState.h"

#include <cassert>

namespace vk2 {
DynamicState::DynamicState(const std::vector<vk::DynamicState>& dynamicStates) 
    : mDynamicStates(dynamicStates)
{
    mCreateInfo.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
    mCreateInfo.pDynamicStates = mDynamicStates.empty() ? nullptr : mDynamicStates.data();
}

const vk::PipelineDynamicStateCreateInfo&
DynamicState::state() const {
    return mCreateInfo;
}

DynamicState::DynamicState(const DynamicState& state)
    : mCreateInfo(state.mCreateInfo)
    , mDynamicStates(state.mDynamicStates) 
{
    mCreateInfo.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
    mCreateInfo.pDynamicStates = mDynamicStates.empty() ? nullptr : mDynamicStates.data();
}
const DynamicState& 
DynamicState::operator=(const DynamicState& state) {
    if (this == &state) {
        return *this;
    }

    mCreateInfo = state.mCreateInfo;
    mDynamicStates = state.mDynamicStates;
    mCreateInfo.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
    mCreateInfo.pDynamicStates = mDynamicStates.empty() ? nullptr : mDynamicStates.data();

    return *this;
}
}