#ifndef UTILS_PIPELINE_STAGE_COLOR_BLEND_ATTACHMENT_STATE
#define UTILS_PIPELINE_STAGE_COLOR_BLEND_ATTACHMENT_STATE

#include <vulkan/vulkan.hpp>

namespace vk2 {
//
// PipelineColorBlendAttachmentState wrapper
//
// After a fragment shader has returned a color, it needs to be combined with 
// the color that is already in the framebuffer.
//
// This transformation is known as color blending and there are two ways to do it:
// - Mix the old and new value to produce a final color
// - Combine the oldand new value using a bitwise operation
//
// There are two types of structs to configure color blending:
// - vk::PipelineColorBlendAttachmentState contains the configuration 
//   per attached framebuffer
// - vk::PipelineColorBlendStateCreateInfo contains the global color blending settings.
//
// You need this class to:
// - Create the ColorBlendState
//
class ColorBlendAttachmentState {
public:
    // The default constructor values disable color blending for the attachment.
    ColorBlendAttachmentState(const vk::Bool32 blendEnable = VK_FALSE,
                              const vk::ColorComponentFlags colorWriteMask =
                              vk::ColorComponentFlagBits::eR |
                              vk::ColorComponentFlagBits::eG |
                              vk::ColorComponentFlagBits::eB |
                              vk::ColorComponentFlagBits::eA,
                              const vk::BlendFactor srcColorBlendFactor = vk::BlendFactor::eZero,
                              const vk::BlendFactor dstColorBlendFactor = vk::BlendFactor::eZero,
                              const vk::BlendOp colorBlendOp = vk::BlendOp::eAdd,
                              const vk::BlendFactor srcAlphaBlendFactor = vk::BlendFactor::eZero,
                              const vk::BlendFactor dstAlphaBlendFactor = vk::BlendFactor::eZero,
                              const vk::BlendOp alphaBlendOp = vk::BlendOp::eAdd);

    const vk::PipelineColorBlendAttachmentState&
    state() const;

    void
    disableColorBlending();

    void
    enableColorBlending();

private:
    vk::PipelineColorBlendAttachmentState mCreateInfo = {};
};
}

#endif 