#include "PipelineStateFactory.h"

namespace vk {
void 
PipelineStateFactory::emptyVertexInputState(VkPipelineVertexInputStateCreateInfo& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.vertexBindingDescriptionCount = 0;
    createInfo.vertexAttributeDescriptionCount = 0;
}

void
PipelineStateFactory::vertexInputState(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
                                       const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions,
                                       VkPipelineVertexInputStateCreateInfo& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    createInfo.pVertexBindingDescriptions = bindingDescriptions.empty() ? nullptr : bindingDescriptions.data();
    createInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    createInfo.pVertexAttributeDescriptions = attributeDescriptions.empty() ? nullptr : attributeDescriptions.data();
}

void
PipelineStateFactory::defaultRasterizationState(VkPipelineRasterizationStateCreateInfo& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // If true, then frafments that are beyond the near and far planes
    // are clamped to them as opposed to discarding them.
    createInfo.depthClampEnable = VK_FALSE;
    // If true, then geometry never passes through the rasterizer stage.
    createInfo.rasterizerDiscardEnable = VK_FALSE;
    // Determine how fragments are generated for geometry.
    // VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments.
    // VK_POLYGON_MODE_LINE: polygon edges are drawn as lines.
    // VK_POLYGON_MODE_POINT: polygon vertices are drwan as points.
    createInfo.polygonMode = VK_POLYGON_MODE_FILL;
    // Describe the thickness of lines in terms of number of fragments.
    // The maximum line width that is supported depends on the hardware
    // and any line thicker than 1.0f requires you to enable the 
    // wideLines GPU feature.
    createInfo.lineWidth = 1.0f;
    // Determine the type of face culling to use. You can disable
    // culling, cull the front faces, cull the back faces or both.
    createInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    // Specify the vertex order for faces to be considered
    // front-facing and can be clockwise or counterclockwise.
    createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // The rasterizer can alter the depth values by adding
    // a constant value or biasing them based on a fragment's slope.
    // We do not use this.
    createInfo.depthBiasEnable = VK_FALSE;
}

void
PipelineStateFactory::disableMultisampleState(VkPipelineMultisampleStateCreateInfo& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo.sampleShadingEnable = VK_FALSE;
    createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.minSampleShading = 1.0f;
    createInfo.pSampleMask = nullptr;
    createInfo.alphaToCoverageEnable = VK_FALSE;
    createInfo.alphaToOneEnable = VK_FALSE;
}

void
PipelineStateFactory::createInputAssemblyState(const VkPrimitiveTopology primitiveTopology,
                                               const VkBool32 primitiveRestartEnable,
                                               VkPipelineInputAssemblyStateCreateInfo& createInfo) {
    // VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
    // VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every two vertices without reuse
    // VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: the end vertex of every line is 
    // used as start vertex for the next line.
    // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: triangle from every 3 vertices
    // without reuse.
    // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: the second and third vertex of
    // every triangle are used as first two vertices of the next triangle.

    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    // What kind of geometry will be drawn from the vertices
    createInfo.topology = primitiveTopology;
    // If the primitive restart should be enabled or not.
    createInfo.primitiveRestartEnable = primitiveRestartEnable;
}

void
PipelineStateFactory::defaultDepthStencilState(VkPipelineDepthStencilStateCreateInfo& createInfo) {
    createInfo = {};
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
}

void
PipelineStateFactory::disableColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& state) {
    state = {};
    state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    state.blendEnable = VK_FALSE;
}

void
PipelineStateFactory::enableColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& state) {
    state = {};
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
}

void
PipelineStateFactory::colorBlendState(const VkPipelineColorBlendAttachmentState& colorBlendAttachmentState,
                                      VkPipelineColorBlendStateCreateInfo& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorBlendAttachmentState;
}

void
PipelineStateFactory::colorBlendState(const std::vector<VkPipelineColorBlendAttachmentState>& colorBlendAttachmentStates,
                                      VkPipelineColorBlendStateCreateInfo& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
    createInfo.pAttachments = colorBlendAttachmentStates.data();
}
}