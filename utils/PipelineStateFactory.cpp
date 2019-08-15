#include "PipelineStateFactory.h"

namespace vk {
VkPipelineVertexInputStateCreateInfo 
PipelineStateFactory::emptyVertexInputState() {
    VkPipelineVertexInputStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.vertexBindingDescriptionCount = 0;
    createInfo.pVertexBindingDescriptions = nullptr;
    createInfo.vertexAttributeDescriptionCount = 0;
    createInfo.pVertexAttributeDescriptions = nullptr;

    return createInfo;
}

VkPipelineRasterizationStateCreateInfo
PipelineStateFactory::defaultRasterizationState() {
    VkPipelineRasterizationStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo.depthClampEnable = VK_FALSE;
    createInfo.rasterizerDiscardEnable = VK_FALSE;
    createInfo.polygonMode = VK_POLYGON_MODE_FILL;
    createInfo.lineWidth = 1.0f;
    createInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    createInfo.depthBiasEnable = VK_FALSE;
    createInfo.depthBiasConstantFactor = 0.0f;
    createInfo.depthBiasClamp = 0.0f;
    createInfo.depthBiasSlopeFactor = 0.0f;

    return createInfo;
}

VkPipelineMultisampleStateCreateInfo
PipelineStateFactory::disableMultisampleState() {
    VkPipelineMultisampleStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo.sampleShadingEnable = VK_FALSE;
    createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.minSampleShading = 1.0f;
    createInfo.pSampleMask = nullptr;
    createInfo.alphaToCoverageEnable = VK_FALSE;
    createInfo.alphaToOneEnable = VK_FALSE;

    return createInfo;
}

VkPipelineInputAssemblyStateCreateInfo
PipelineStateFactory::createInputAssemblyState(const VkPrimitiveTopology primitiveTopology,
                                               const VkBool32 primitiveRestartEnable) {
    VkPipelineInputAssemblyStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    createInfo.topology = primitiveTopology;
    createInfo.primitiveRestartEnable = primitiveRestartEnable;

    return createInfo;
}

VkPipelineDepthStencilStateCreateInfo
PipelineStateFactory::defaultDepthStencilState() {
    VkPipelineDepthStencilStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    createInfo.depthTestEnable = VK_TRUE;
    createInfo.depthWriteEnable = VK_TRUE;
    createInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    createInfo.depthBoundsTestEnable = VK_FALSE;
    createInfo.minDepthBounds = 0.0f;
    createInfo.maxDepthBounds = 1.0f;
    createInfo.stencilTestEnable = VK_FALSE;
    createInfo.front = {};
    createInfo.back = {};

    return createInfo;
}

VkPipelineColorBlendAttachmentState
PipelineStateFactory::disableColorBlendAttachmentState() {
    VkPipelineColorBlendAttachmentState state = {};
    state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    state.blendEnable = VK_FALSE;

    return state;
}

VkPipelineColorBlendAttachmentState
PipelineStateFactory::enableColorBlendAttachmentState() {
    VkPipelineColorBlendAttachmentState state = {};
    state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    state.blendEnable = VK_TRUE;
    state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    state.colorBlendOp = VK_BLEND_OP_ADD;
    state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    state.alphaBlendOp = VK_BLEND_OP_ADD;

    return state;
}

VkPipelineColorBlendStateCreateInfo
PipelineStateFactory::colorBlendState(const VkPipelineColorBlendAttachmentState& colorBlendAttachmentState) {
    VkPipelineColorBlendStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorBlendAttachmentState;

    return createInfo;
}

VkPipelineColorBlendStateCreateInfo
PipelineStateFactory::colorBlendState(const std::vector<VkPipelineColorBlendAttachmentState>& colorBlendAttachmentStates) {
    VkPipelineColorBlendStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
    createInfo.pAttachments = colorBlendAttachmentStates.data();

    return createInfo;
}
}