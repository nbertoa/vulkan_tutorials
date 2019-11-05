#include "VertexInputState.h"

namespace vk {
VertexInputState::VertexInputState(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
                                   const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions) 
    : mBindingDescriptions(bindingDescriptions)
    , mAttributeDescriptions(attributeDescriptions)
{
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    mCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(mBindingDescriptions.size());
    mCreateInfo.pVertexBindingDescriptions = mBindingDescriptions.empty() ? nullptr : mBindingDescriptions.data();
    mCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mAttributeDescriptions.size());
    mCreateInfo.pVertexAttributeDescriptions = mAttributeDescriptions.empty() ? nullptr : mAttributeDescriptions.data();
}

const VkPipelineVertexInputStateCreateInfo& 
VertexInputState::vkState() const {
    return mCreateInfo;
}

VertexInputState::VertexInputState(const VertexInputState& state)
    : mCreateInfo(state.mCreateInfo)
    , mBindingDescriptions(state.mBindingDescriptions)
    , mAttributeDescriptions (state.mAttributeDescriptions){
    mCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(mBindingDescriptions.size());
    mCreateInfo.pVertexBindingDescriptions = mBindingDescriptions.empty() ? nullptr : mBindingDescriptions.data();
    mCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mAttributeDescriptions.size());
    mCreateInfo.pVertexAttributeDescriptions = mAttributeDescriptions.empty() ? nullptr : mAttributeDescriptions.data();
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
    mCreateInfo.pVertexBindingDescriptions = mBindingDescriptions.empty() ? nullptr : mBindingDescriptions.data();
    mCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mAttributeDescriptions.size());
    mCreateInfo.pVertexAttributeDescriptions = mAttributeDescriptions.empty() ? nullptr : mAttributeDescriptions.data();

    return *this;
}
}