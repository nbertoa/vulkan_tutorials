#ifndef UTILS_PIPELINE_STAGE_DYNAMIC_STATE
#define UTILS_PIPELINE_STAGE_DYNAMIC_STATE

#include <vector>
#include <vulkan/vulkan.hpp>

namespace vulkan {
//
// PipelineDynamicStateCreateInfo  wrapper
//
// A dynamic pipeline state is a state that can be changed by 
// a command buffer command during the execution of a command buffer. 
//
// Advance notification of what states are dynamic during command buffer 
// execution may be useful for a driver as it sets up the GPU for command buffer execution.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class DynamicState {
public:
    // * dynamicStates Indicate which dynamic state is taken 
    //   from dynamic state commands
    DynamicState(const std::vector<vk::DynamicState>& dynamicStates = {});
    DynamicState(const DynamicState& state);
    const DynamicState& operator=(const DynamicState& state);

    const vk::PipelineDynamicStateCreateInfo&
    state() const;

private:
    vk::PipelineDynamicStateCreateInfo  mCreateInfo = {};

    std::vector<vk::DynamicState> mDynamicStates;
};
}

#endif 