#include "VertexInputState.h"

namespace vulkan {
VertexInputState::VertexInputState(const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions,
                                   const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions) 
    : mBindingDescriptions(bindingDescriptions)
    , mAttributeDescriptions(attributeDescriptions)
{
    mCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(mBindingDescriptions.size());
    mCreateInfo.pVertexBindingDescriptions = 
        mBindingDescriptions.empty() ? nullptr : mBindingDescriptions.data();
    mCreateInfo.vertexAttributeDescriptionCount = 
        static_cast<uint32_t>(mAttributeDescriptions.size());
    mCreateInfo.pVertexAttributeDescriptions = 
        mAttributeDescriptions.empty() ? nullptr : mAttributeDescriptions.data();
}

const vk::PipelineVertexInputStateCreateInfo& 
VertexInputState::state() const {
    return mCreateInfo;
}

VertexInputState::VertexInputState(const VertexInputState& state)
    : mCreateInfo(state.mCreateInfo)
    , mBindingDescriptions(state.mBindingDescriptions)
    , mAttributeDescriptions (state.mAttributeDescriptions){
    mCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(mBindingDescriptions.size());
    mCreateInfo.pVertexBindingDescriptions = 
        mBindingDescriptions.empty() ? nullptr : mBindingDescriptions.data();
    mCreateInfo.vertexAttributeDescriptionCount = 
        static_cast<uint32_t>(mAttributeDescriptions.size());
    mCreateInfo.pVertexAttributeDescriptions = 
        mAttributeDescriptions.empty() ? nullptr : mAttributeDescriptions.data();
}

const VertexInputState& 
VertexInputState::operator=(const VertexInputState& state) {
    if (this == &state) {
        return *this;
    }

    mCreateInfo = state.mCreateInfo;
    mBindingDescriptions = state.mBindingDescriptions;
    mAttributeDescriptions = state.mAttributeDescriptions;
    mCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(mBindingDescriptions.size());
    mCreateInfo.pVertexBindingDescriptions = 
        mBindingDescriptions.empty() ? nullptr : mBindingDescriptions.data();
    mCreateInfo.vertexAttributeDescriptionCount = 
        static_cast<uint32_t>(mAttributeDescriptions.size());
    mCreateInfo.pVertexAttributeDescriptions = 
        mAttributeDescriptions.empty() ? nullptr : mAttributeDescriptions.data();

    return *this;
}
}