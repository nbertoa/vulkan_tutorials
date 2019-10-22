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
    ColorBlendState(const ColorBlendAttachmentState& colorBlendAttachmentState);

    const VkPipelineColorBlendStateCreateInfo&
    vkState() const;

private:
    VkPipelineColorBlendStateCreateInfo mCreateInfo = {};
};
}

#endif 