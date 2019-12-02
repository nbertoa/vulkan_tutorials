#ifndef UTILS_PIPELINE_STAGE_TESSELLATION_STATE
#define UTILS_PIPELINE_STAGE_TESSELLATION_STATE

#include <vulkan/vulkan.hpp>

namespace vk2 {
//
// VkPipelineTessellationStateCreateInfo wrapper
//
// Tessellation involves three pipeline stages:
//
// First, a tessellation control shader transforms control points 
// of a patch and can produce per-patch data.
//
// Second, a fixed-function tessellator generates multiple primitives 
// corresponding to a tessellation of the patch in(u, v) or (u, v, w) parameter space.
//
// Third, a tessellation evaluation shader transforms the vertices of the tessellated patch, 
// for example to compute their positions and attributes as part of the tessellated surface.
//
// The tessellator is enabled when the pipeline contains both a 
// tessellation control shaderand a tessellation evaluation shader.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class TessellationState {
public:
    // * patchControlPoints count per patch.
    TessellationState(const uint32_t patchControlPoints = 1);
    
    const VkPipelineTessellationStateCreateInfo&
    vkState() const;
    
private:
    VkPipelineTessellationStateCreateInfo mCreateInfo = {};
};
}

#endif 