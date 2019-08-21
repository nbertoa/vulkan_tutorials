#ifndef UTILS_GRAPHICS_PIPELINE
#define UTILS_GRAPHICS_PIPELINE

#include <cassert>

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "PipelineLayout.h"

namespace vk {
class LogicalDevice;
class RenderPass;

// Graphics VkPipeline wrapper to be able to create/destroy/handle it easily.
// The graphics pipeline consist of the following:
// - Shader stages: the shader modules that define the functionality
//   of the programmable stages of the graphics pipeline.
// - Fixed-function state: all of the structures that define the 
//   fixed-function stages of the pipeline, like input assembly, rasterizer,
//   viewport and color blending.
// - Pipeline layout: the uniform and push values referenced by 
//   the shader that can be updated at draw time.
// - Render pass: the attachments referenced by the pipeline 
//   stages and their usage.
class GraphicsPipeline {
public:
    // Notes:
    // - Ownership of pipelineLayout will be transferred
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
                     const VkPipelineDynamicStateCreateInfo* dynamicState);
    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline&) = delete;
    const GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    VkPipeline vkPipeline() const {
        assert(mPipeline != VK_NULL_HANDLE);
        return mPipeline;
    }

private:
    const LogicalDevice& mLogicalDevice;
    std::unique_ptr<PipelineLayout> mPipelineLayout;
    VkPipeline mPipeline = VK_NULL_HANDLE;
};
}

#endif
