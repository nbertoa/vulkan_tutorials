#ifndef UTILS_PIPELINE_RASTERIZATION_STATE_FACTORY
#define UTILS_PIPELINE_RASTERIZATION_STATE_FACTORY

#include <vulkan/vulkan.h>

namespace vk {
// The rasterizer takes the geometry that is shaped by the vertices 
// from the vertex shader and turns into fragments to be colored by the fragment shader.
class PipelineRasterizationStateFactory {
public:
    PipelineRasterizationStateFactory() = delete;
    ~PipelineRasterizationStateFactory() = delete;
    PipelineRasterizationStateFactory(const PipelineRasterizationStateFactory&) = delete;
    const PipelineRasterizationStateFactory& operator=(const PipelineRasterizationStateFactory&) = delete;
    PipelineRasterizationStateFactory(PipelineRasterizationStateFactory&&) = delete;
    PipelineRasterizationStateFactory& operator=(PipelineRasterizationStateFactory&&) = delete;
    
    // No depth clamping
    // Fill polygon
    // Back face culling
    // Clockwise Front-facing
    // No depth value biasing
    static VkPipelineRasterizationStateCreateInfo defaultCreateInfo();
};
}

#endif