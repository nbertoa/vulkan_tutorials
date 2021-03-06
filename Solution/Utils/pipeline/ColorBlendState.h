#ifndef UTILS_PIPELINE_STAGE_COLOR_BLEND_STATE
#define UTILS_PIPELINE_STAGE_COLOR_BLEND_STATE

#include <vulkan/vulkan.hpp>

#include "ColorBlendAttachmentState.h"

namespace vulkan {
//
// PipelineColorBlendStateCreateInfo wrapper
//
// After a fragment shader has returned a color, it needs to be combined with 
// the color that is already in the framebuffer.
//
// This transformation is known as color blending and there are two ways to do it:
// - Mix the old and new value to produce a final color
// - Combine the oldand new value using a bitwise operation
//
// There are two types of structs to configure color blending:
// - PipelineColorBlendAttachmentState contains the configuration 
//   per attached framebuffer
// - PipelineColorBlendStateCreateInfo contains the global color blending settings.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class ColorBlendState {
public:
    // * enableLogicalOperation controls whether to apply logical operation.
    //
    // * logicalOperation selects which logical operation to apply
    ColorBlendState(const ColorBlendAttachmentState& colorBlendAttachmentState = {},
                    const vk::Bool32 logicOpEnable = VK_FALSE,
                    const vk::LogicOp logicalOperation = vk::LogicOp::eClear);
    ColorBlendState(const ColorBlendState& state);
    const ColorBlendState& operator=(const ColorBlendState& state);

    const vk::PipelineColorBlendStateCreateInfo&
    state() const;

private:
    vk::PipelineColorBlendStateCreateInfo mCreateInfo = {};

    ColorBlendAttachmentState mColorBlendAttachmentState;
};
}

#endif 