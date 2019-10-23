#ifndef UTILS_PIPELINE_STAGE_DYNAMIC_STATE
#define UTILS_PIPELINE_STAGE_DYNAMIC_STATE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
//
// VkPipelineDynamicStateCreateInfo  wrapper
//
// A dynamic pipeline state is a state that can be changed by a command buffer command 
// during the execution of a command buffer. Advance notification of what states are dynamic during 
// command buffer execution may be useful for a driver as it sets up the GPU for command buffer execution.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class DynamicState {
public:
    // * dynamicStates Indicate which dynamic state is taken from dynamic state commands
    //
    //   - VK_DYNAMIC_STATE_VIEWPORT specifies that the pViewports state in 
    //     VkPipelineViewportStateCreateInfo will be ignored and must be set dynamically with 
    //     vkCmdSetViewport before any draw commands. The number of viewports used by a pipeline is 
    //     still specified by the viewportCount member of VkPipelineViewportStateCreateInfo.
    //   - VK_DYNAMIC_STATE_SCISSOR specifies that the pScissors state in VkPipelineViewportStateCreateInfo 
    //     will be ignoredand must be set dynamically with vkCmdSetScissor before any draw commands.
    //     The number of scissor rectangles used by a pipeline is still specified by the scissorCount 
    //     member of VkPipelineViewportStateCreateInfo.
    //   - VK_DYNAMIC_STATE_LINE_WIDTH specifies that the lineWidth state in VkPipelineRasterizationStateCreateInfo 
    //     will be ignoredand must be set dynamically with vkCmdSetLineWidth before any draw commands 
    //     that generate line primitives for the rasterizer.
    //   - VK_DYNAMIC_STATE_DEPTH_BIAS specifies that the depthBiasConstantFactor, depthBiasClamp and 
    //     depthBiasSlopeFactor states in VkPipelineRasterizationStateCreateInfo will be ignoredand 
    //     must be set dynamically with vkCmdSetDepthBias before any draws are performed with depthBiasEnable 
    //     in VkPipelineRasterizationStateCreateInfo set to VK_TRUE.
    //   - VK_DYNAMIC_STATE_BLEND_CONSTANTS specifies that the blendConstants state in 
    //     VkPipelineColorBlendStateCreateInfo will be ignored and must be set dynamically with 
    //     vkCmdSetBlendConstants before any draws are performed with a pipeline state with 
    //     VkPipelineColorBlendAttachmentState member blendEnable set to VK_TRUEand any of the 
    //     blend functions using a constant blend color.
    //   - VK_DYNAMIC_STATE_DEPTH_BOUNDS specifies that the minDepthBoundsand maxDepthBounds states 
    //     of VkPipelineDepthStencilStateCreateInfo will be ignoredand must be set dynamically with 
    //     vkCmdSetDepthBounds before any draws are performed with a pipeline state with 
    //     VkPipelineDepthStencilStateCreateInfo member depthBoundsTestEnable set to VK_TRUE.
    //   - VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK specifies that the compareMask state in 
    //     VkPipelineDepthStencilStateCreateInfo for both front and back will be ignoredand must be set 
    //     dynamically with vkCmdSetStencilCompareMask before any draws are performed with a pipeline 
    //     state with VkPipelineDepthStencilStateCreateInfo member stencilTestEnable set to VK_TRUE
    //   - VK_DYNAMIC_STATE_STENCIL_WRITE_MASK specifies that the writeMask state in 
    //     VkPipelineDepthStencilStateCreateInfo for both frontand back will be ignoredand must be 
    //     set dynamically with vkCmdSetStencilWriteMask before any draws are performed with a pipeline state 
    //     with VkPipelineDepthStencilStateCreateInfo member stencilTestEnable set to VK_TRUE
    //   - VK_DYNAMIC_STATE_STENCIL_REFERENCE specifies that the reference state in VkPipelineDepthStencilStateCreateInfo 
    //     for both frontand back will be ignoredand must be set dynamically with vkCmdSetStencilReference before 
    //     any draws are performed with a pipeline state with VkPipelineDepthStencilStateCreateInfo 
    //     member stencilTestEnable set to VK_TRUE
    //   - VK_DYNAMIC_STATE_VIEWPORT_W_SCALING_NV specifies that the pViewportScalings state in 
    //     VkPipelineViewportWScalingStateCreateInfoNV will be ignored and must be set dynamically with 
    //     vkCmdSetViewportWScalingNV before any draws are performed with a pipeline state with 
    //     VkPipelineViewportWScalingStateCreateInfoNV member viewportScalingEnable set to VK_TRUE
    //   - VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT specifies that the pDiscardRectangles state in 
    //     VkPipelineDiscardRectangleStateCreateInfoEXT will be ignoredand must be set dynamically with 
    //     vkCmdSetDiscardRectangleEXT before any draw or clear commands.The VkDiscardRectangleModeEXTand 
    //     the number of active discard rectangles is still specified by the discardRectangleMode and discardRectangleCount 
    //     members of VkPipelineDiscardRectangleStateCreateInfoEXT.
    //   - VK_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT specifies that the sampleLocationsInfo state in 
    //     VkPipelineSampleLocationsStateCreateInfoEXT will be ignoredand must be set dynamically with 
    //     vkCmdSetSampleLocationsEXT before any draw or clear commands.Enabling custom sample locations is 
    //     still indicated by the sampleLocationsEnable member of VkPipelineSampleLocationsStateCreateInfoEXT.
    //   - VK_DYNAMIC_STATE_EXCLUSIVE_SCISSOR_NV specifies that the pExclusiveScissors state in 
    //     VkPipelineViewportExclusiveScissorStateCreateInfoNV will be ignoredand must be set dynamically with 
    //     vkCmdSetExclusiveScissorNV before any draw commands.The number of exclusive scissor rectangles used 
    //     by a pipeline is still specified by the exclusiveScissorCount member of VkPipelineViewportExclusiveScissorStateCreateInfoNV.
    //   - VK_DYNAMIC_STATE_VIEWPORT_SHADING_RATE_PALETTE_NV specifies that the pShadingRatePalettes state in 
    //     VkPipelineViewportShadingRateImageStateCreateInfoNV will be ignoredand must be set dynamically with 
    //     vkCmdSetViewportShadingRatePaletteNV before any draw commands.
    //   - VK_DYNAMIC_STATE_VIEWPORT_COARSE_SAMPLE_ORDER_NV specifies that the coarse sample order state in 
    //     VkPipelineViewportCoarseSampleOrderStateCreateInfoNV will be ignoredand must be set dynamically with 
    //     vkCmdSetCoarseSampleOrderNV before any draw commands.
    //   - VK_DYNAMIC_STATE_LINE_STIPPLE_EXT specifies that the lineStippleFactorand lineStipplePattern state in 
    //     VkPipelineRasterizationLineStateCreateInfoEXT will be ignoredand must be set dynamically with 
    //     vkCmdSetLineStippleEXT before any draws are performed with a pipeline state with 
    //     VkPipelineRasterizationLineStateCreateInfoEXT member stippledLineEnable set to VK_TRUE.
    DynamicState(const std::vector<VkDynamicState>& dynamicStates);

    const VkPipelineDynamicStateCreateInfo&
    vkState() const;

private:
    VkPipelineDynamicStateCreateInfo  mCreateInfo = {};
};
}

#endif 