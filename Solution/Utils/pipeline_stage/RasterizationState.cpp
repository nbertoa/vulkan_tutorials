#include "RasterizationState.h"

namespace vk2 {
RasterizationState::RasterizationState(const vk::Bool32 depthClampEnable,
                                       const vk::Bool32 rasterizerDiscardEnable,
                                       const vk::PolygonMode polygonMode,
                                       const float lineWidth,
                                       const vk::CullModeFlags cullMode,
                                       const vk::FrontFace frontFace,
                                       const vk::Bool32 depthBiasEnable,
                                       const float depthBiasConstantFactor,
                                       const float depthBiasClamp,
                                       const float depthBiasSlopeFactor) {
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

const vk::PipelineRasterizationStateCreateInfo&
RasterizationState::state() const {
    return mCreateInfo;
}
}