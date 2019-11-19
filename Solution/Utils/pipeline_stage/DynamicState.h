#ifndef UTILS_PIPELINE_STAGE_DYNAMIC_STATE
#define UTILS_PIPELINE_STAGE_DYNAMIC_STATE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
//
// VkPipelineDynamicStateCreateInfo  wrapper
//
// A dynamic pipeline state is a state that can be changed by a command buffer command 
// during the execution of a command buffer. Advance notification of what states 
// are dynamic during command buffer execution may be useful for a driver as it sets up 
// the GPU for command buffer execution.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class DynamicState {
public:
    // * dynamicStates Indicate which dynamic state is taken 
    //   from dynamic state commands (VK_DYNAMIC_STATE_):
    //
    //   - VIEWPORT, SCISSOR, LINE_WIDTH, DEPTH_BIAS, BLEND_CONSTANTS, DEPTH_BOUNDS, 
    //     STENCIL_COMPARE_MASK, STENCIL_WRITE_MASK, STENCIL_REFERENCE, VIEWPORT_W_SCALING_NV,
    //     DISCARD_RECTANGLE_EXT, SAMPLE_LOCATIONS_EXT, EXCLUSIVE_SCISSOR_NV, 
    //     VIEWPORT_SHADING_RATE_PALETTE_NV, VIEWPORT_COARSE_SAMPLE_ORDER_NV, 
    //     LINE_STIPPLE_EXT.
    DynamicState(const std::vector<VkDynamicState>& dynamicStates = {});
    DynamicState(const DynamicState& state);
    const DynamicState& operator=(const DynamicState& state);

    const VkPipelineDynamicStateCreateInfo&
    vkState() const;

private:
    VkPipelineDynamicStateCreateInfo  mCreateInfo = {};

    std::vector<VkDynamicState> mDynamicStates;
};
}

#endif 