﻿#ifndef UTILS_PIPELINE_STAGE_DEPTH_STENCIL_STATE
#define UTILS_PIPELINE_STAGE_DEPTH_STENCIL_STATE

#include <vulkan/vulkan.h>

namespace vk {
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
    // * depthWriteEnable controls whether depth writes are enabled when depthTestEnable is VK_TRUE.
    //   Depth writes are always disabled when depthTestEnable is VK_FALSE.
    //
    // * depthCompareOp is the comparison operator used in the depth test:
    //
    //   - VK_COMPARE_OP_NEVER specifies that the test never passes.
    //   - VK_COMPARE_OP_LESS specifies that the test passes when R < S.
    //   - VK_COMPARE_OP_EQUAL specifies that the test passes when R = S.
    //   - VK_COMPARE_OP_LESS_OR_EQUAL specifies that the test passes when R ≤ S.
    //   - VK_COMPARE_OP_GREATER specifies that the test passes when R > S.
    //   - VK_COMPARE_OP_NOT_EQUAL specifies that the test passes when R ≠ S.
    //   - VK_COMPARE_OP_GREATER_OR_EQUAL specifies that the test passes when R ≥ S.
    //   - VK_COMPARE_OP_ALWAYS specifies that the test always passes.
    //
    // * depthBoundsTestEnable controls whether depth bounds testing is enabled.
    //
    // * stencilTestEnable controls whether stencil testing is enabled.
    //
    // * front and back control the parameters of the stencil test:
    //
    //   - failOp specifying the action performed on samples that fail the stencil test.
    //   - passOp specifying the action performed on samples that pass both the depth and stencil tests.
    //   - depthFailOp specifying the action performed on samples that 
    //     pass the stencil test and fail the depth test.
    //   - compareOp specifying the comparison operator used in the stencil test.
    //   - compareMask selects the bits of the unsigned integer stencil values participating in the stencil test.
    //   - writeMask selects the bits of the unsigned integer stencil values 
    //     updated by the stencil test in the stencil framebuffer attachment.
    //   - reference value that is used in the unsigned stencil comparison.
    //
    // * minDepthBounds and maxDepthBounds define the range of values used in the depth bounds test.
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