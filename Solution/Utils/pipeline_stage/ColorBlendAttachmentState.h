#ifndef UTILS_PIPELINE_STAGE_COLOR_BLEND_ATTACHMENT_STATE
#define UTILS_PIPELINE_STAGE_COLOR_BLEND_ATTACHMENT_STATE

#include <vulkan/vulkan.h>

namespace vk {
//
// VkPipelineColorBlendStateCreateInfo wrapper
//
// After a fragment shader has returned a color, it needs to be combined with 
// the color that is already in the framebuffer.
// This transformation is known as color blendingand there are two ways to do it:
// - Mix the old and new value to produce a final color
// - Combine the oldand new value using a bitwise operation
//
// There are two types of structs to configure color blending.
// The first struct, VkPipelineColorBlendAttachmentState contains the configuration 
// per attached framebuffer and the second struct, 
// VkPipelineColorBlendStateCreateInfo contains the global color blending settings.
//
// You need this class to:
// - Create the ColorBlendState
//
class ColorBlendAttachmentState {
public:
    // The default constructor values disable color blending for the attachment.
    ColorBlendAttachmentState(const VkBool32 blendEnable = VK_FALSE,
                              const VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                                           VK_COLOR_COMPONENT_G_BIT |
                                                                           VK_COLOR_COMPONENT_B_BIT |
                                                                           VK_COLOR_COMPONENT_A_BIT,
                              const VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                              const VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                              const VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
                              const VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                              const VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                              const VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD);

    const VkPipelineColorBlendAttachmentState&
    vkState() const;

    void
    disableColorBlending();

    void
    enableColorBlending();

private:
    VkPipelineColorBlendAttachmentState mCreateInfo = {};
};
}

#endif 