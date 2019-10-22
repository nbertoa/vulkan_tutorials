#include "VertexInputState.h"

namespace vk {
VertexInputState::VertexInputState(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
                                   const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions) {
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    mCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    mCreateInfo.pVertexBindingDescriptions = bindingDescriptions.empty() ? nullptr : bindingDescriptions.data();
    mCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    mCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.empty() ? nullptr : attributeDescriptions.data();
}

const VkPipelineVertexInputStateCreateInfo& 
VertexInputState::vkState() const {
    return mCreateInfo;
}
}