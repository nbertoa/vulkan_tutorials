#include "RasterizationState.h"

namespace vk2 {
RasterizationState::RasterizationState(const VkBool32 depthClampEnable,
                                       const VkBool32 rasterizerDiscardEnable,
                                       const VkPolygonMode polygonMode,
                                       const float lineWidth,
                                       const VkCullModeFlags cullMode,
                                       const VkFrontFace frontFace,
                                       const VkBool32 depthBiasEnable,
                                       const float depthBiasConstantFactor,
                                       const float depthBiasClamp,
                                       const float depthBiasSlopeFactor) {
    mCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    mCreateInfo.depthClampEnable = depthClampEnable;
    mCreateInfo.rasterizerDiscardEnable = rasterizerDiscardEnable;
    mCreateInfo.polygonMode = polygonMode;
    mCreateInfo.lineWidth = lineWidth;
    mCreateInfo.cullMode = cullMode;
    mCreateInfo.frontFace = frontFace;
    mCreateInfo.depthBiasEnable = depthBiasEnable;
    mCreateInfo.depthBiasConstantFactor = depthBiasConstantFactor;
    mCreateInfo.depthBiasClamp = depthBiasClamp;
    mCreateInfo.depthBiasSlopeFactor = depthBiasSlopeFactor;
}

const VkPipelineRasterizationStateCreateInfo&
RasterizationState::vkState() const {
    return mCreateInfo;
}
}