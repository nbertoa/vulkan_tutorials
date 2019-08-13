#ifndef UTILS_PIPELINE_DEPTH_STENCIL_STATE_FACTORY
#define UTILS_PIPELINE_DEPTH_STENCIL_STATE_FACTORY

#include <vulkan/vulkan.h>

namespace vk {
class PipelineDepthStencilStateFactory {
public:
    PipelineDepthStencilStateFactory() = delete;
    ~PipelineDepthStencilStateFactory() = delete;
    PipelineDepthStencilStateFactory(const PipelineDepthStencilStateFactory&) = delete;
    const PipelineDepthStencilStateFactory& operator=(const PipelineDepthStencilStateFactory&) = delete;
    PipelineDepthStencilStateFactory(PipelineDepthStencilStateFactory&&) = delete;
    PipelineDepthStencilStateFactory& operator=(PipelineDepthStencilStateFactory&&) = delete;
    
    // Enable depth test
    // Disable stencil test
    static VkPipelineDepthStencilStateCreateInfo defaultState();
};
}

#endif
