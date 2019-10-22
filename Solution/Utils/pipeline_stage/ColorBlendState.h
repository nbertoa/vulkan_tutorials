#ifndef UTILS_PIPELINE_STAGE_COLOR_BLEND_STATE
#define UTILS_PIPELINE_STAGE_COLOR_BLEND_STATE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class ColorBlendAttachmentState;

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
// - Create the GraphicsPipeline
//
class ColorBlendState {
public:
    // * enableLogicalOperation controls whether to apply logical operation.
    //
    // * logicalOperation selects which logical operation to apply:
    //
    //   - VK_LOGIC_OP_CLEAR 
    //   - VK_LOGIC_OP_AND 
    //   - VK_LOGIC_OP_AND_REVERSE 
    //   - VK_LOGIC_OP_COPY 
    //   - VK_LOGIC_OP_AND_INVERTED 
    //   - VK_LOGIC_OP_NO_OP 
    //   - VK_LOGIC_OP_XOR 
    //   - VK_LOGIC_OP_OR
    //   - VK_LOGIC_OP_NOR
    //   - VK_LOGIC_OP_EQUIVALENT
    //   - VK_LOGIC_OP_INVERT
    //   - VK_LOGIC_OP_OR_REVERSE
    //   - VK_LOGIC_OP_COPY_INVERTED
    //   - VK_LOGIC_OP_OR_INVERTED
    //   - VK_LOGIC_OP_NAND
    //   - VK_LOGIC_OP_SET
    //   - VK_LOGIC_OP_MAX_ENUM
    ColorBlendState(const ColorBlendAttachmentState& colorBlendAttachmentState,
                    const VkBool32 logicOpEnable = VK_FALSE,
                    const VkLogicOp logicalOperation = VK_LOGIC_OP_CLEAR);

    const VkPipelineColorBlendStateCreateInfo&
    vkState() const;

private:
    VkPipelineColorBlendStateCreateInfo mCreateInfo = {};
};
}

#endif 