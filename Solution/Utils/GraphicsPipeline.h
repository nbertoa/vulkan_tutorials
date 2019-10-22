#ifndef UTILS_GRAPHICS_PIPELINE
#define UTILS_GRAPHICS_PIPELINE

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "PipelineLayout.h"
#include "pipeline_stage/ColorBlendState.h"
#include "pipeline_stage/InputAssemblyState.h"
#include "pipeline_stage/MultisampleState.h"
#include "pipeline_stage/RasterizationState.h"
#include "pipeline_stage/VertexInputState.h"

namespace vk {
class LogicalDevice;
class RenderPass;

//
// Graphics VkPipeline wrapper.
//
// The pipeline is the big object which composes most of the objects 
// listed in the other classes of this project.
//
// It represents the configuration of the whole pipeline and has a lot of parameters.
// One of them is PipelineLayout (it defines the layout of descriptorsand push constants).
// There are two types of Pipelines: ComputePipeline and GraphicsPipeline.
//
// ComputePipeline is the simpler one, because all it supports is compute-only 
// programs (sometimes called compute shaders).
//
// GraphicsPipeline is much more complex, and it encompasses:
// - Shader stages: the shader modules that define the functionality
//   of the programmable stages of the graphics pipeline
//   (vertex, fragment, geometry, compute and tessellation where applicable)
// - Fixed-function state: all of the structures that define the 
//   fixed-function stages of the pipeline, like input assembly, rasterizer,
//   viewport and color blending.
// - Pipeline layout: the uniform and push values referenced by 
//   the shader that can be updated at draw time.
// - Render pass: the attachments referenced by the pipeline 
//   stages and their usage.
//
// All those parameters that used to be separate settings in much older graphics APIs (DirectX 9, OpenGL), 
// were later grouped into a smaller number of state objects as the APIs progressed (DirectX 10 and 11) and 
// must now be baked into a single big, immutable object with today’s modern APIs like Vulkan.
//
// For each different set of parameters needed during rendering you must create a new Pipeline.
// You can then set it as the current active Pipeline in a CommandBuffer by calling the function vkCmdBindPipeline.
//
// You need the GraphicsPipeline to:
// - CommandBuffer execution of the command vkCmbBindPipeline
//
// To create/use the GraphicsPipeline you need:
// - ShaderModule
// - PipelineLayout
// - RenderPass
//
class GraphicsPipeline {
public:
    // Notes:
    // - Ownership of pipelineLayout will be transferred.
    // - The parameter of pointer types are optional (nullptr).
    //
    // * renderPass describes the environment in which the pipeline will be used; 
    //   the pipeline must only be used with an instance of any render pass compatible with the one provided.
    //
    // * subPassIndex in the render pass where this pipeline will be used.
    //
    // * layout is the description of binding locations used by both 
    //   the pipeline and descriptor sets used with the pipeline.
    //
    // * shaderStages to be included in the graphics pipeline (Read ShaderModule to understand this structure)
    //
    // * vertexInputState which specifies vertex input attribute and 
    //   vertex input binding descriptions. 
    //
    // * inputAssemblyState which specifies what kind of geometry will be 
    //   drawn from the vertices and if primitive restart should be enabled.
    //
    // * viewportState (Read SwapChain to understand this structure, which can be obtained from it)
    //
    // * rasterizationState configures the rasterizer which takes the geometry that 
    //   is shaped by the vertices from the vertex shader and turns it into fragments 
    //   to be colored by the fragment shader. It also performs depth testing, 
    //   face culling and the scissor test, and it can be configured to output fragments that fill entire 
    //   polygons or just the edges(wireframe rendering).
    //   
    // * multisampleState configures multisampling, which is one of the ways to perform anti-aliasing.

    // * depthStencilState is an optional VkPipelineDepthStencilStateCreateInfo structure, 
    //   and is ignored if the pipeline has rasterization disabled or if the subpass of the render pass of the pipeline 
    //   is created against does not use a depth / stencil attachment:
    //
    //   - depthTestEnable controls whether depth testing is enabled.
    //   - depthWriteEnable controls whether depth writes are enabled when depthTestEnable is VK_TRUE.
    //     Depth writes are always disabled when depthTestEnable is VK_FALSE.
    //   - depthCompareOp is the comparison operator used in the depth test.
    //   - depthBoundsTestEnable controls whether depth bounds testing is enabled.
    //   - stencilTestEnable controls whether stencil testing is enabled.
    //   - front and back control the parameters of the stencil test.
    //   - minDepthBounds and maxDepthBounds define the range of values used in the depth bounds test.
    //
    // * colorBlendState is an optional VkPipelineColorBlendStateCreateInfo, 
    //   and is ignored if the pipeline has rasterization disabled or if the subpass of the render pass of the pipeline 
    //   is created against does not use any color attachments:
    //
    //   - logicOpEnable controls whether to apply Logical Operations.
    //   - logicOp selects which logical operation to apply.
    //   - attachmentCount in pAttachments.
    //     This value must equal the colorAttachmentCount for the subpass in which this pipeline is used.
    //   - pAttachments array of per target attachment states:
    //     . blendEnable controls whether blending is enabled for the corresponding color attachment. 
    //       If blending is not enabled, the source fragment’s color for that attachment is passed through unmodified.
    //     . srcColorBlendFactor selects which blend factor is used to determine the source factors(Sr, Sg, Sb).
    //     . dstColorBlendFactor selects which blend factor is used to determine the destination factors(Dr, Dg, Db).
    //     . colorBlendOp selects which blend operation is used to calculate the RGB values to write to the color attachment.
    //     . srcAlphaBlendFactor selects which blend factor is used to determine the source factor Sa.
    //     . dstAlphaBlendFactor selects which blend factor is used to determine the destination factor Da.
    //     . alphaBlendOp selects which blend operation is use to calculate the alpha values to write to the color attachment.
    //     . colorWriteMask specifies which of the R, G, B, and /or A components are enabled for writing:
    //       x VK_COLOR_COMPONENT_R_BIT specifies that the R value is written to the color attachment for the appropriate sample. 
    //         Otherwise, the value in memory is unmodified.
    //       x VK_COLOR_COMPONENT_G_BIT specifies that the G value is written to the color attachment for the appropriate sample.
    //         Otherwise, the value in memory is unmodified.
    //       x VK_COLOR_COMPONENT_B_BIT specifies that the B value is written to the color attachment for the appropriate sample.
    //         Otherwise, the value in memory is unmodified.
    //       x VK_COLOR_COMPONENT_A_BIT specifies that the A value is written to the color attachment for the appropriate sample.
    //         Otherwise, the value in memory is unmodified.
    //   - blendConstants used as the R, G, B, and A components 
    //     of the blend constant that are used in blending, depending on the blend factor.
    //
    // * dynamicState is an optional VkPipelineDynamicStateCreateInfo, and is used to indicate which 
    //   properties of the pipeline state object are dynamic and can be changed independently of the pipeline state.
    //   This can be nullptr, which means no state in the pipeline is considered dynamic:
    //
    //   - dynamicStateCount in the pDynamicStates array.
    //   - pDynamicStates array specifies which pieces of 
    //     pipeline state will use the values from dynamic state commands rather than from pipeline state creation info:
    //     . VK_DYNAMIC_STATE_VIEWPORT specifies that the pViewports state in VkPipelineViewportStateCreateInfo will be 
    //       ignored and must be set dynamically with vkCmdSetViewport before any draw commands. 
    //       The number of viewports used by a pipeline is still specified by the viewportCount 
    //       member of VkPipelineViewportStateCreateInfo.
    //     . VK_DYNAMIC_STATE_SCISSOR specifies that the pScissors state in VkPipelineViewportStateCreateInfo will be ignored 
    //       and must be set dynamically with vkCmdSetScissor before any draw commands.The number of scissor rectangles used 
    //       by a pipeline is still specified by the scissorCount member of VkPipelineViewportStateCreateInfo.
    //     . VK_DYNAMIC_STATE_LINE_WIDTH specifies that the lineWidth state in VkPipelineRasterizationStateCreateInfo will be ignored
    //       and must be set dynamically with vkCmdSetLineWidth before any draw commands that generate line primitives for the rasterizer.
    //     . VK_DYNAMIC_STATE_DEPTH_BIAS specifies that the depthBiasConstantFactor, depthBiasClampand depthBiasSlopeFactor states 
    //       in VkPipelineRasterizationStateCreateInfo will be ignoredand must be set dynamically with vkCmdSetDepthBias before 
    //       any draws are performed with depthBiasEnable in VkPipelineRasterizationStateCreateInfo set to VK_TRUE.
    //     . VK_DYNAMIC_STATE_BLEND_CONSTANTS specifies that the blendConstants state in VkPipelineColorBlendStateCreateInfo 
    //       will be ignored and must be set dynamically with vkCmdSetBlendConstants before any draws are performed with 
    //       a pipeline state with VkPipelineColorBlendAttachmentState member blendEnable set to VK_TRUEand any of the blend 
    //       functions using a constant blend color.
    //     . VK_DYNAMIC_STATE_DEPTH_BOUNDS specifies that the minDepthBoundsand maxDepthBounds states of 
    //       VkPipelineDepthStencilStateCreateInfo will be ignoredand must be set dynamically with vkCmdSetDepthBounds before 
    //       any draws are performed with a pipeline state with VkPipelineDepthStencilStateCreateInfo member 
    //       depthBoundsTestEnable set to VK_TRUE.
    //     . VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK specifies that the compareMask state in VkPipelineDepthStencilStateCreateInfo 
    //       for both front and back will be ignored and must be set dynamically with vkCmdSetStencilCompareMask before 
    //       any draws are performed with a pipeline state with VkPipelineDepthStencilStateCreateInfo 
    //       member stencilTestEnable set to VK_TRUE
    //     . VK_DYNAMIC_STATE_STENCIL_WRITE_MASK specifies that the writeMask state in VkPipelineDepthStencilStateCreateInfo 
    //       for both front and back will be ignored and must be set dynamically with vkCmdSetStencilWriteMask before 
    //       any draws are performed with a pipeline state with VkPipelineDepthStencilStateCreateInfo 
    //       member stencilTestEnable set to VK_TRUE
    //     . VK_DYNAMIC_STATE_STENCIL_REFERENCE specifies that the reference state in VkPipelineDepthStencilStateCreateInfo 
    //       for both front and back will be ignored and must be set dynamically with vkCmdSetStencilReference before 
    //       any draws are performed with a pipeline state with VkPipelineDepthStencilStateCreateInfo 
    //       member stencilTestEnable set to VK_TRUE
    //     . VK_DYNAMIC_STATE_VIEWPORT_W_SCALING_NV specifies that the pViewportScalings state in 
    //       VkPipelineViewportWScalingStateCreateInfoNV will be ignored and must be set dynamically with vkCmdSetViewportWScalingNV 
    //       before any draws are performed with a pipeline state with VkPipelineViewportWScalingStateCreateInfoNV 
    //       member viewportScalingEnable set to VK_TRUE
    //     . VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT specifies that the pDiscardRectangles state in 
    //       VkPipelineDiscardRectangleStateCreateInfoEXT will be ignored and must be set dynamically with vkCmdSetDiscardRectangleEXT 
    //       before any draw or clear commands. The VkDiscardRectangleModeEXTand the number of active discard rectangles is still 
    //       specified by the discardRectangleModeand discardRectangleCount members of VkPipelineDiscardRectangleStateCreateInfoEXT.
    //     . VK_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT specifies that the sampleLocationsInfo state in 
    //       VkPipelineSampleLocationsStateCreateInfoEXT will be ignored and must be set dynamically with 
    //       vkCmdSetSampleLocationsEXT before any draw or clear commands.Enabling custom sample locations is 
    //       still indicated by the sampleLocationsEnable member of VkPipelineSampleLocationsStateCreateInfoEXT.
    //     . VK_DYNAMIC_STATE_EXCLUSIVE_SCISSOR_NV specifies that the pExclusiveScissors state in 
    //       VkPipelineViewportExclusiveScissorStateCreateInfoNV will be ignored and must be set dynamically with 
    //       vkCmdSetExclusiveScissorNV before any draw commands.The number of exclusive scissor rectangles used by 
    //       a pipeline is still specified by the exclusiveScissorCount member of VkPipelineViewportExclusiveScissorStateCreateInfoNV.
    //     . VK_DYNAMIC_STATE_VIEWPORT_SHADING_RATE_PALETTE_NV specifies that the pShadingRatePalettes state in 
    //       VkPipelineViewportShadingRateImageStateCreateInfoNV will be ignored and must be set dynamically with 
    //       vkCmdSetViewportShadingRatePaletteNV before any draw commands.
    //     . VK_DYNAMIC_STATE_VIEWPORT_COARSE_SAMPLE_ORDER_NV specifies that the coarse sample order state in 
    //       VkPipelineViewportCoarseSampleOrderStateCreateInfoNV will be ignored and must be set dynamically with 
    //       vkCmdSetCoarseSampleOrderNV before any draw commands.
    //     . VK_DYNAMIC_STATE_LINE_STIPPLE_EXT specifies that the lineStippleFactorand lineStipplePattern state 
    //       in VkPipelineRasterizationLineStateCreateInfoEXT will be ignored and must be set dynamically with 
    //       vkCmdSetLineStippleEXT before any draws are performed with a pipeline state with 
    //       VkPipelineRasterizationLineStateCreateInfoEXT member stippledLineEnable set to VK_TRUE. 
    //
    // * tessellationState is an optional VkPipelineTessellationStateCreateInfo structure, 
    //   and is ignored if the pipeline does not include a tessellation control shader stage
    //   and tessellation evaluation shader stage:
    //   - patchControlPoints
    GraphicsPipeline(const LogicalDevice& logicalDevice,
                     const RenderPass& renderPass,
                     const uint32_t subPassIndex,
                     std::unique_ptr<PipelineLayout>& pipelineLayout,
                     const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
                     const ColorBlendState* colorBlendState,                     
                     const InputAssemblyState* inputAssemblyState,                     
                     const MultisampleState* multisampleState,
                     const RasterizationState* rasterizationState,
                     const VertexInputState* vertexInputState,
                     const VkPipelineViewportStateCreateInfo* viewportState,
                     const VkPipelineDepthStencilStateCreateInfo* depthStencilState,                     
                     const VkPipelineDynamicStateCreateInfo* dynamicState,
                     const VkPipelineTessellationStateCreateInfo* tessellationState);
    ~GraphicsPipeline();
    GraphicsPipeline(GraphicsPipeline&& other) noexcept;
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    const GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    VkPipeline 
    vkPipeline() const;

private:
    const LogicalDevice& mLogicalDevice;
    std::unique_ptr<PipelineLayout> mPipelineLayout;
    VkPipeline mPipeline = VK_NULL_HANDLE;
};
}

#endif
