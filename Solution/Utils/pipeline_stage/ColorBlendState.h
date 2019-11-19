#ifndef UTILS_PIPELINE_STAGE_COLOR_BLEND_STATE
#define UTILS_PIPELINE_STAGE_COLOR_BLEND_STATE

#include <vulkan/vulkan.h>

#include "ColorBlendAttachmentState.h"

namespace vk {
//
// VkPipelineColorBlendStateCreateInfo wrapper
//
// After a fragment shader has returned a color, it needs to be combined with 
// the color that is already in the framebuffer.
// This transformation is known as color blending and there are two ways to do it:
// - Mix the old and new value to produce a final color
// - Combine the oldand new value using a bitwise operation
//
// There are two types of structs to configure color blending.
// The first struct, VkPipelineColorBlendAttachmentState contains the configuration 
// per attached framebuffer and the second struct, 
// VkPipelineColorBlendStateCreateInfo contains the global color blending settings.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class ColorBlendState {
public:
    // * enableLogicalOperation controls whether to apply logical operation.
    //
    // * logicalOperation selects which logical operation to apply (VK_LOGIC_OP_):
    //
    //   - CLEAR, AND, AND_REVERSE, COPY, AND_INVERTED, NO_OP, XOR, OR, NOR, EQUIVALENT, INVERT,
    //     OR_REVERSE, COPY_INVERTED, OR_INVERTED, NAND, SET, MAX_ENUM
    ColorBlendState(const ColorBlendAttachmentState& colorBlendAttachmentState = {},
                    const VkBool32 logicOpEnable = VK_FALSE,
                    const VkLogicOp logicalOperation = VK_LOGIC_OP_CLEAR);
    ColorBlendState(const ColorBlendState& state);
    const ColorBlendState& operator=(const ColorBlendState& state);

    const VkPipelineColorBlendStateCreateInfo&
    vkState() const;

private:
    VkPipelineColorBlendStateCreateInfo mCreateInfo = {};

    ColorBlendAttachmentState mColorBlendAttachmentState;
};
}

#endif 