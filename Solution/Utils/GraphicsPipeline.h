#ifndef UTILS_GRAPHICS_PIPELINE
#define UTILS_GRAPHICS_PIPELINE

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "PipelineLayout.h"

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
    // * shaderStages to be included in the graphics pipeline:
    //
    //   - flags bitmask that specifies how the pipeline shader stage will be generated:
    //     . VK_PIPELINE_SHADER_STAGE_CREATE_ALLOW_VARYING_SUBGROUP_SIZE_BIT_EXT specifies 
    //       that the SubgroupSize may vary in the shader stage.
    //     . VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT_EXT specifies that the subgroup 
    //       sizes must be launched with all invocations active in the compute stage.
    //   - stage specifies a single pipeline stage:
    //     . VK_SHADER_STAGE_VERTEX_BIT
    //     . VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
    //     . VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
    //     . VK_SHADER_STAGE_GEOMETRY_BIT
    //     . VK_SHADER_STAGE_FRAGMENT_BIT
    //     . VK_SHADER_STAGE_COMPUTE_BIT
    //     . VK_SHADER_STAGE_TASK_BIT_NV.
    //     . VK_SHADER_STAGE_MESH_BIT_NV
    //     . VK_SHADER_STAGE_ALL_GRAPHICS is a combination of bits used as shorthand to specify all 
    //       graphics stages defined above(excluding the compute stage).
    //     . VK_SHADER_STAGE_ALL is a combination of bits used as shorthand to specify all shader stages 
    //       supported by the device, including all additional stages which are introduced by extensions.
    //     . VK_SHADER_STAGE_RAYGEN_BIT_NV specifies the ray generation stage.
    //     . VK_SHADER_STAGE_ANY_HIT_BIT_NV
    //     . VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV
    //     . VK_SHADER_STAGE_MISS_BIT_NV
    //     . VK_SHADER_STAGE_INTERSECTION_BIT_NV
    //     . VK_SHADER_STAGE_CALLABLE_BIT_NV
    //   - module containing the shader for this stage.
    //   - pName specifies the entry point name of the shader for this stage.
    //   - pSpecializationInfo is an optiona VkSpecializationInfo :
    //     . mapEntryCount
    //     . pMapEntries which map constant IDs to offsets in pData.
    //     . dataSize
    //     . pData contains the actual constant values to specialize with.
    //
    // * vertexInputState is an optional VkPipelineVertexInputStateCreateInfo.
    //   It is ignored if the pipeline includes a mesh shader stage:
    //
    //   - vertexBindingDescriptionCount provided in pVertexBindingDescriptions.
    //   - pVertexBindingDescriptions array:
    //     . binding number that this structure describes.
    //     . stride is the distance in bytes between two consecutive elements within the buffer.
    //     . inputRate specifies whether vertex attribute addressing is a 
    //       function of the vertex index or of the instance index.
    //   - vertexAttributeDescriptionCount provided in pVertexAttributeDescriptions.
    //   - pVertexAttributeDescriptions array:
    //     . location is the shader binding location number for this attribute.
    //     . binding number which this attribute takes its data from.
    //     . format is the size and type of the vertex attribute data.
    //     . offset in bytes of this attribute relative to the start of an element in the vertex input binding. 
    //
    // * inputAssemblyState is an optional VkPipelineInputAssemblyStateCreateInfo which 
    //   determines input assembly behavior. It is ignored if the pipeline includes a mesh shader stage:
    //   - topology defining the primitive topology:
    //     . VK_PRIMITIVE_TOPOLOGY_POINT_LIST specifies a series of separate point primitives.
    //     . VK_PRIMITIVE_TOPOLOGY_LINE_LIST specifies a series of separate line primitives.
    //     . VK_PRIMITIVE_TOPOLOGY_LINE_STRIP specifies a series of connected line primitives 
    //       with consecutive lines sharing a vertex.
    //     . VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST specifies a series of separate triangle primitives.
    //     . VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP specifies a series of connected triangle 
    //       primitives with consecutive triangles sharing an edge.
    //     . VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN specifies a series of connected triangle 
    //       primitives with all triangles sharing a common vertex.
    //     . VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY specifies a series 
    //       separate line primitives with adjacency.
    //     . VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY specifies a series connected line primitives 
    //       with adjacency, with consecutive primitives sharing three vertices.
    //     . VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY specifies a series separate 
    //       triangle primitives with adjacency.
    //     . VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY specifies connected triangle primitives 
    //       with adjacency, with consecutive triangles sharing an edge.
    //     . VK_PRIMITIVE_TOPOLOGY_PATCH_LIST specifies separate patch primitives.
    //   - primitiveRestartEnable controls whether a special vertex index value is treated as restarting the 
    //     assembly of primitives. This enable only applies to indexed draws (vkCmdDrawIndexed and vkCmdDrawIndexedIndirect), 
    //     and the special index value is either 0xFFFFFFFF when the indexType parameter of vkCmdBindIndexBuffer 
    //     is equal to VK_INDEX_TYPE_UINT32, 0xFF when indexType is equal to VK_INDEX_TYPE_UINT8_EXT, or 0xFFFF when 
    //     indexType is equal to VK_INDEX_TYPE_UINT16. Primitive restart is not allowed for “list” topologies.
    //
    // * viewportState is an optional VkPipelineViewportStateCreateInfo, 
    //   and is ignored if the pipeline has rasterization disabled:
    //
    //   - viewportCount used by the pipeline.
    //   - pViewports defining the viewport transforms.
    //     If the viewport state is dynamic, this member is ignored.
    //   - scissorCount must match the number of viewports.
    //   - pScissors defining the rectangular bounds of the 
    //     scissor for the corresponding viewport. If the scissor state is dynamic, this member is ignored.
    //
    // * rasterizationState is an optional VkPipelineRasterizationStateCreateInfo:
    //
    //   - depthClampEnable controls whether to clamp the fragment’s depth values. 
    //     If the pipeline is not created with VkPipelineRasterizationDepthClipStateCreateInfoEXT present 
    //     then enabling depth clamp will also disable clipping primitives to the z planes of the frustrum. 
    //     Otherwise depth clipping is controlled by the state set in VkPipelineRasterizationDepthClipStateCreateInfoEXT.
    //   - rasterizerDiscardEnable controls whether primitives are discarded immediately before the rasterization stage.
    //   - polygonMode is the triangle rendering mode:
    //     . VK_POLYGON_MODE_POINT specifies that polygon vertices are drawn as points.
    //     . VK_POLYGON_MODE_LINE specifies that polygon edges are drawn as line segments.
    //     . VK_POLYGON_MODE_FILL specifies that polygons are rendered using the polygon rasterization rules.
    //     . VK_POLYGON_MODE_FILL_RECTANGLE_NV specifies that polygons are rendered using polygon rasterization rules, 
    //       modified to consider a sample within the primitive if the sample location is inside the axis-aligned bounding box 
    //       of the triangle after projection. Note that the barycentric weights used in attribute interpolation can extend 
    //       outside the range[0, 1] when these primitives are shaded.Special treatment is given to a sample position on the 
    //       boundary edge of the bounding box. In such a case, if two rectangles lie on either side of a common edge 
    //       (with identical endpoints) on which a sample position lies, then exactly one of the triangles must produce a 
    //       fragment that covers that sample during rasterization.
    //       Polygons rendered in VK_POLYGON_MODE_FILL_RECTANGLE_NV mode may be clipped by the frustum or by user clip planes.
    //       If clipping is applied, the triangle is culled rather than clipped.
    //   - cullMode is the triangle facing direction used for primitive culling:
    //     . VK_CULL_MODE_NONE specifies that no triangles are discarded
    //     . VK_CULL_MODE_FRONT_BIT specifies that front-facing triangles are discarded
    //     . VK_CULL_MODE_BACK_BIT specifies that back-facing triangles are discarded
    //     . VK_CULL_MODE_FRONT_AND_BACK specifies that all triangles are discarded.
    //   - frontFace that specifies the front-facing triangle orientation to be used for culling:
    //     . VK_FRONT_FACE_COUNTER_CLOCKWISE specifies that a triangle with positive area is considered front-facing.
    //     . VK_FRONT_FACE_CLOCKWISE specifies that a triangle with negative area is considered front-facing.
    //   - depthBiasEnable controls whether to bias fragment depth values.
    //   - depthBiasConstantFactor controlling the constant depth value added to each fragment.
    //   - depthBiasClamp is the maximum(or minimum) depth bias of a fragment.
    //   - depthBiasSlopeFactor applied to a fragment’s slope in depth bias calculations.
    //   - lineWidth of rasterized line segments.
    //   
    // * multisampleState is an optional VkPipelineMultisampleStateCreateInfo, 
    //   and is ignored if the pipeline has rasterization disabled:
    //
    //   - rasterizationSamples specifies the number of samples per pixelused in rasterization:
    //     . VK_SAMPLE_COUNT_1_BIT
    //     . VK_SAMPLE_COUNT_2_BIT
    //     . VK_SAMPLE_COUNT_4_BIT
    //     . VK_SAMPLE_COUNT_8_BIT
    //     . VK_SAMPLE_COUNT_16_BIT
    //     . VK_SAMPLE_COUNT_32_BIT
    //     . VK_SAMPLE_COUNT_64_BIT
    //   - sampleShadingEnable
    //   - minSampleShading fraction if sampleShadingEnable is set to VK_TRUE.
    //   - pSampleMask of static coverage information that is ANDed with the coverage information 
    //     generated during rasterizationk.
    //   - alphaToCoverageEnable controls whether a temporary coverage value is generated based on the alpha component 
    //     of the fragment’s first color output.
    //   - alphaToOneEnable controls whether the alpha component of the fragment’s first color 
    //     output is replaced with one.
    //
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
                     const VkPipelineVertexInputStateCreateInfo* vertexInputState,
                     const VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState,
                     const VkPipelineViewportStateCreateInfo* viewportState,
                     const VkPipelineRasterizationStateCreateInfo* rasterizationState,
                     const VkPipelineMultisampleStateCreateInfo* multisampleState,
                     const VkPipelineDepthStencilStateCreateInfo* depthStencilState,
                     const VkPipelineColorBlendStateCreateInfo* colorBlendState,
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
