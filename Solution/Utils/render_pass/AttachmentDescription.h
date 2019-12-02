#ifndef UTILS_RENDER_PASS_ATTACHMENT_DESCRIPTION
#define UTILS_RENDER_PASS_ATTACHMENT_DESCRIPTION

#include <vulkan/vulkan.hpp>

namespace vk2 {
//
// VkAttachmentDescription wrapper
//
// We need to tell to the render pass the description of the attachments
// (frame buffers) that we are going to use.
//
// You need the AttachmentDescription to:
// - Create the RenderPass.
//
class AttachmentDescription {
public:
    // * format of the image view that will be used for the attachment.
    //
    // * sampleCount of the image(VK_SAMPLE_COUNT_):
    //
    //   . 1_BIT, 2_BIT, 4_BIT, 8_BIT, 16_BIT, 32_BIT, 64_BIT
    //
    // * colorAndDepthLoadOperation specifying how the contents of color and 
    //   depth components of the attachment are treated at the beginning of the 
    //   subpass where it is first used.
    // * stencilLoadOperation specifying how the contents of stencil components of the attachment 
    //   are treated at the beginning of the 
    //   subpass where it is first used (VK_ATTACHMENT_LOAD_OP_):
    //
    //   - LOAD, CLEAR, DONT_CARE
    //
    // * colorAndDepthStoreOperation specifying how the contents of color and 
    //   depth components of the attachment are treated at the end of the subpass 
    //   where it is last used:
    // * stencilStoreOperation specifying how the contents of stencil 
    //   components of the attachment are treated at the end of 
    //   the last subpass where it is used (VK_ATTACHMENT_STORE_OP_):
    //
    //   - STORE, DONT_CARE
    //
    // * initialLayout that the attachment image subresource will be in when a 
    //   render pass instance begins:
    // * finalLayout that the attachment image subresource will be transitioned to 
    //   when a render pass instance ends (VK_IMAGE_LAYOUT_):
    //
    //   - UNDEFINED, PREINITIALIZED, GENERAL, DEPTH_STENCIL_ATTACHMENT_BIT, 
    //     DEPTH_STENCIL_READ_ONLY_OPTIMAL, DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL, 
    //     DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL, SHADER_READ_ONLY_OPTIMAL, 
    //     TRANSFER_SRC_OPTIMAL, TRANSFER_DST_OPTIMAL, PRESENT_SRC_KHR, SHARED_PRESENT_KHR, 
    //     SHADING_RATE_OPTIMAL_NV, FRAGMENT_DENSITY_MAP_OPTIMAL_EXT,
    //
    // Notes:
    // If the attachment uses a color format, then colorAndDepthLoadOperation and 
    // colorAndDepthStoreOperation are used, 
    // and stencilLoadOperation stencilStoreOperation are ignored.
    // If the format has depth and/or stencil components, colorAndDepthLoadOperation and 
    // colorAndDepthStoreOperation apply only to the depth data, 
    // while stencilLoadOpand stencilStoreOp define how the stencil data is handled. 
    // colorAndDepthLoadOperation and stencilLoadOperation define the load operations 
    // that execute as part of the first subpass that uses the attachment.
    // colorAndDepthStoreOperation and stencilStoreOperation define the store operations 
    // that execute as part of the last subpass that uses the attachment.
    //
    // The load operation for each sample in an attachment happens-before any recorded 
    // command which accesses the sample in the first 
    // subpass where the attachment is used. 
    // Load operations for attachments with a depth/stencil format execute in the 
    // VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT pipeline stage. 
    // Load operations for attachments with a color format execute in the 
    // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT pipeline stage.
    //
    // The store operation for each sample in an attachment happens-after any 
    // recorded command which accesses the sample in the last 
    // subpass where the attachment is used. 
    // Store operations for attachments with a depth/stencil format execute in the 
    // VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT pipeline stage.
    // Store operations for attachments with a color format execute in the 
    // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT pipeline stage.
    //
    // If an attachment is not used by any subpass, then colorAndDepthLoadOperation, 
    // colorAndDepthStoreOperation, stencilLoadOperation, and stencilStoreOperation are ignored, 
    // and the attachment’s memory contents will not be modified by execution 
    // of a render pass instance.
    //
    // The load and store operations apply on the first and last use of each view in the 
    // render pass, respectively.
    // If a view index of an attachment is not included in the view mask in any subpass 
    // that uses it, then the load and store operations are ignored, and the 
    // attachment’s memory contents will not be modified by execution of a render pass instance.
    //
    // During a render pass instance, input/color attachments with color formats that 
    // have a component size of 8, 16, or 32 bits must be 
    // represented in the attachment’s format throughout the instance. 
    // Attachments with other floating-or fixed-point color formats, 
    // or with depth components may be represented in a format with a precision higher 
    // than the attachment format, but must be represented with the same range. 
    // When such a component is loaded via the loadOp, it will be converted into an 
    // implementation-dependent format used by the render pass. Such components must 
    // be converted from the render pass format, to the format of the attachment, 
    // before they are resolved or stored at the end of a render pass instance via 
    // colorAndDepthStoreOperation. 
    //
    // If flags includes VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT, then the attachment is 
    // treated as if it shares physical memory with another attachment in the same render pass. 
    // This information limits the ability of the implementation to reorder 
    // certain operations(like layout transitions and the colorAndDepthStoreOperation) 
    // such that it is not improperly reordered against other uses of the same physical 
    // memory via a different attachment.
    AttachmentDescription(const VkFormat format,
                          const VkAttachmentLoadOp colorAndDepthLoadOperation,
                          const VkAttachmentStoreOp colorAndDepthStoreOperation,
                          const VkImageLayout initialLayout,
                          const VkImageLayout finalLayout,
                          const VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT, // No multisampling
                          const VkAttachmentLoadOp stencilLoadOperation = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                          const VkAttachmentStoreOp stencilStoreOperation = VK_ATTACHMENT_STORE_OP_DONT_CARE);
       
private:
    VkAttachmentDescription mAttachmentDescription = {};
};
}

#endif 