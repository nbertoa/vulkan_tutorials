#ifndef UTILS_PIPELINE_STAGE_DEPTH_STENCIL_STATE
#define UTILS_PIPELINE_STAGE_DEPTH_STENCIL_STATE

#include <vulkan/vulkan.h>

namespace vk2 {
//
// VkPipelineDepthStencilStateCreateInfo wrapper
//
// If you are using a depth and/or stencil buffer, then you also need to 
// configure the depth and stencil tests using VkPipelineDepthStencilStateCreateInfo.
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
    // * depthCompareOp is the comparison operator used in the depth test (VK_COMPARE_OP_):
    //
    //   - NEVER, LESS, EQUAL, LESS_OR_EQUAL, GREATER, NOT_EQUAL, GREATER_OR_EQUAL, ALWAYS
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
    DepthStencilState(const VkBool32 depthTestEnable = VK_TRUE,
                      const VkBool32 depthWriteEnable = VK_TRUE,
                      const VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS,
                      const VkBool32 depthBoundsTestEnable = VK_FALSE,
                      const VkBool32 stencilTestEnable = VK_FALSE,
                      const VkStencilOpState front = {},
                      const VkStencilOpState back = {},
                      const float minDepthBounds = 0.0f,
                      const float maxDepthBounds = 1.0f);

    const VkPipelineDepthStencilStateCreateInfo&
    vkState() const;

    void
    enableDepthDisableStencil();

private:
    VkPipelineDepthStencilStateCreateInfo mCreateInfo = {};
};
}

#endif 