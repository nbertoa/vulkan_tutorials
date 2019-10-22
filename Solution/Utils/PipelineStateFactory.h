#ifndef UTILS_PIPELINE_VERTEX_INPUT_STATE_FACTORY
#define UTILS_PIPELINE_VERTEX_INPUT_STATE_FACTORY

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
// Factory to easily get most used pipeline states.
class PipelineStateFactory {
public:
    PipelineStateFactory() = delete;
    ~PipelineStateFactory() = delete;
    PipelineStateFactory(const PipelineStateFactory&) = delete;
    const PipelineStateFactory& operator=(const PipelineStateFactory&) = delete;
    PipelineStateFactory(PipelineStateFactory&&) = delete;
    PipelineStateFactory& operator=(PipelineStateFactory&&) = delete;        
        
    // Depth and stencil tests:
    // VkPipelineDepthStencilStateCreateInfo:
    // - sType is the type of this structure.
    // - pNext is NULL or a pointer to an extension - specific structure.
    // - flags is reserved for future use.
    // - depthTestEnable controls whether depth testing is enabled.
    // - depthWriteEnable controls whether depth writes are enabled when depthTestEnable is VK_TRUE.
    //   Depth writes are always disabled when depthTestEnable is VK_FALSE.
    // - depthCompareOp is the comparison operator used in the depth test.
    // - depthBoundsTestEnable controls whether depth bounds testing is enabled.
    // - stencilTestEnable controls whether stencil testing is enabled.
    // - frontand back control the parameters of the stencil test.
    // - minDepthBoundsand maxDepthBounds define the range of values used in the depth bounds test.
        
    // Enable depth test
    // Disable stencil test
    static void 
    defaultDepthStencilState(VkPipelineDepthStencilStateCreateInfo& depthStencilStateCreateInfo);
};
}

#endif
