#ifndef UTILS_PIPELINE_GRAPHICS_PIPELINE
#define UTILS_PIPELINE_GRAPHICS_PIPELINE

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "PipelineLayout.h"

namespace vk2 {
class DynamicState;
class PipelineStates;
class RenderPass;
class ShaderStages;

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
// All those parameters that used to be separate settings in much older graphics APIs 
// (DirectX 9, OpenGL), were later grouped into a smaller number of state objects as the 
// APIs progressed (DirectX 10 and 11) and must now be baked into a single big, 
// immutable object with today’s modern APIs like Vulkan.
//
// For each different set of parameters needed during rendering you must create a new Pipeline.
// You can then set it as the current active Pipeline in a CommandBuffer by calling the 
// function vkCmdBindPipeline.
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
    // * pipelineLayout is the description of binding locations used by both 
    //   the pipeline and descriptor sets used with the pipeline.
    //   pipelineLayout will be "moved" to this instance.
    //
    // * pipelineStates (Read PipelineStates to understand)
    //
    // * renderPass describes the environment in which the pipeline will be used; 
    //   the pipeline must only be used with an instance of any render pass compatible 
    //   with the one provided.
    //
    // * subPassIndex in the render pass where this pipeline will be used.
    //
    // Notes:
    // If any shader stage fails to compile, the compile log will be reported back 
    // to the application, and VK_ERROR_INVALID_SHADER_NV will be generated.
    //
    // The global logical device is the device that creates the graphics pipeline.
    GraphicsPipeline(PipelineLayout& pipelineLayout,
                     const PipelineStates& pipelineStates,
                     const ShaderStages& shaderStages,
                     const RenderPass& renderPass,
                     const uint32_t subPassIndex = 0);
    ~GraphicsPipeline();
    GraphicsPipeline(GraphicsPipeline&& other) noexcept;
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    const GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    VkPipeline 
    vkPipeline() const;

    const PipelineLayout&
    pipelineLayout() const;

private:
    VkPipeline mPipeline = VK_NULL_HANDLE;
    PipelineLayout mPipelineLayout;
};
}

#endif
