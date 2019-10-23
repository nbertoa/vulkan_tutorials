#ifndef UTILS_GRAPHICS_PIPELINE
#define UTILS_GRAPHICS_PIPELINE

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "PipelineLayout.h"

namespace vk {
class ColorBlendState;
class DepthStencilState;
class DynamicState;
class InputAssemblyState;
class LogicalDevice;
class MultisampleState;
class RasterizationState;
class RenderPass;
class ShaderStages;
class TessellationState;
class VertexInputState;
class ViewportState;

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
    // * pipelineLayout is the description of binding locations used by both 
    //   the pipeline and descriptor sets used with the pipeline.
    //
    // * shaderStages to be included in the graphics pipeline (Read ShaderStage to understand this structure)
    //
    // * colorBlendState defines how the fragment shader returned color needs to be combined with 
    //   the color that is already in the framebuffer.
    //   It is ignored if the pipeline has rasterization disabled or if the subpass of the render pass of the pipeline 
    //   is created against does not use any color attachments.
    //
    // * depthStencilState is ignored if the pipeline has rasterization disabled or if 
    //   the subpass of the render pass of the pipeline is created against does not use a depth/stencil attachment.
    //
    // * dynamicState can be changed by a command buffer command during the execution of a command buffer.
    //   This can be nullptr, which means no state in the pipeline is considered dynamic. 
    //
    // * inputAssemblyState which specifies what kind of geometry will be 
    //   drawn from the vertices and if primitive restart should be enabled.
    //
    // * inputAssemblyState which specifies what kind of geometry will be 
    //   drawn from the vertices and if primitive restart should be enabled.
    //   
    // * multisampleState configures multisampling, which is one of the ways to perform anti-aliasing.
    //
    // * rasterizationState configures the rasterizer which takes the geometry that 
    //   is shaped by the vertices from the vertex shader and turns it into fragments 
    //   to be colored by the fragment shader. It also performs depth testing, 
    //   face culling and the scissor test, and it can be configured to output fragments that fill entire 
    //   polygons or just the edges(wireframe rendering).
    //
    // * tessellationState configures the tessellation pipeline and it is ignored if 
    //   the pipeline does not include a tessellation control shader stage and tessellation evaluation shader stage
    //
    // * vertexInputState which specifies vertex input attribute and 
    //   vertex input binding descriptions. 
    //
    // * viewportState which specifies the viewport that describes the region of the framebuffer
    //   that the output will be rendered to.
    GraphicsPipeline(const LogicalDevice& logicalDevice,
                     const RenderPass& renderPass,
                     const uint32_t subPassIndex,
                     std::unique_ptr<PipelineLayout>& pipelineLayout,
                     const ColorBlendState* colorBlendState,   
                     const DepthStencilState* depthStencilState,
                     const DynamicState* dynamicState,
                     const InputAssemblyState* inputAssemblyState,                     
                     const MultisampleState* multisampleState,
                     const RasterizationState* rasterizationState,
                     const ShaderStages& shaderStages,
                     const TessellationState* tessellationState,
                     const VertexInputState* vertexInputState,
                     const ViewportState* viewportState);
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
