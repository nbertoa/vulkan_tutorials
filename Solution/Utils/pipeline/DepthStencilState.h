#ifndef UTILS_PIPELINE_STAGE_DEPTH_STENCIL_STATE
#define UTILS_PIPELINE_STAGE_DEPTH_STENCIL_STATE

#include <vulkan/vulkan.hpp>

namespace vulkan {
//
// PipelineDepthStencilStateCreateInfo wrapper
//
// If you are using a depth and/or stencil buffer, then you also need to 
// configure the depth and stencil tests using PipelineDepthStencilStateCreateInfo.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class DepthStencilState {
public:
    // * depthTestEnable controls whether depth testing is enabled.
    //
    // * depthWriteEnable controls whether depth writes are enabled 
    //   when depthTestEnable is VK_TRUE.
    //   Depth writes are always disabled when depthTestEnable is VK_FALSE.
    //
    // * depthCompareOp is the comparison operator used in the depth test
    //
    // * depthBoundsTestEnable controls whether depth bounds testing is enabled.
    //
    // * stencilTestEnable controls whether stencil testing is enabled.
    //
    // * front and back control the parameters of the stencil test
    //
    // * minDepthBounds and maxDepthBounds define the range of values used 
    //   in the depth bounds test.
    //
    // The default constructor arguments enable depth test and disable stencil test.
    DepthStencilState(const vk::Bool32 depthTestEnable = VK_TRUE,
                      const vk::Bool32 depthWriteEnable = VK_TRUE,
                      const vk::CompareOp depthCompareOp = vk::CompareOp::eLess,
                      const vk::Bool32 depthBoundsTestEnable = VK_FALSE,
                      const vk::Bool32 stencilTestEnable = VK_FALSE,
                      const vk::StencilOpState front = {},
                      const vk::StencilOpState back = {},
                      const float minDepthBounds = 0.0f,
                      const float maxDepthBounds = 1.0f);

    const vk::PipelineDepthStencilStateCreateInfo&
    state() const;

    void
    enableDepthDisableStencil();

private:
    vk::PipelineDepthStencilStateCreateInfo mCreateInfo = {};
};
}

#endif 