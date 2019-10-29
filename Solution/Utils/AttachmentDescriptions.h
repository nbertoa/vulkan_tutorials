#ifndef UTILS_ATTACHMENT_DESCRIPTIONS
#define UTILS_ATTACHMENT_DESCRIPTIONS

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
//
// VkAttachmentDescription vector wrapper
//
// We need to tell to the render pass the description of the attachments
// (frame buffers) that we are going to use.
//
// You need the AttachmentDescriptions to:
// - Create the RenderPass.
//
class AttachmentDescriptions {
public:
    // * format of the image view that will be used for the attachment.
    //
    // * sampleCount of the image:
    //
    //   - VK_SAMPLE_COUNT_1_BIT
    //   - VK_SAMPLE_COUNT_2_BIT
    //   - VK_SAMPLE_COUNT_4_BIT
    //   - VK_SAMPLE_COUNT_8_BIT
    //   - VK_SAMPLE_COUNT_16_BIT
    //   - VK_SAMPLE_COUNT_32_BIT
    //   - VK_SAMPLE_COUNT_64_BIT
    //
    // * colorAndDepthLoadOperation specifying how the contents of color and depth components of the attachment 
    //   are treated at the beginning of the subpass where it is first used.
    // * stencilLoadOperation specifying how the contents of stencil components of the attachment 
    //   are treated at the beginning of the subpass where it is first used.
    //
    //   - VK_ATTACHMENT_LOAD_OP_LOAD specifies that the previous contents of the image within the render area will be preserved. 
    //     For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT. 
    //     For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_READ_BIT.
    //   - VK_ATTACHMENT_LOAD_OP_CLEAR specifies that the contents within the render area will be cleared to a uniform value, 
    //     which is specified when a render pass instance is begun.
    //     For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
    //     For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
    //   - VK_ATTACHMENT_LOAD_OP_DONT_CARE specifies that the previous contents within the area need not be preserved; 
    //     the contents of the attachment will be undefined inside the render area.
    //     For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
    //     For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
    //
    // * colorAndDepthStoreOperation specifying how the contents of color and 
    //   depth components of the attachment are treated at the end of the subpass where it is last used:
    // * stencilStoreOperation specifying how the contents of stencil 
    //   components of the attachment are treated at the end of the last subpass where it is used:
    //
    //   - VK_ATTACHMENT_STORE_OP_STORE specifies the contents generated during the render pass and within the render area 
    //     are written to memory. For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
    //     For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
    //   - VK_ATTACHMENT_STORE_OP_DONT_CARE specifies the contents within the render area are not needed after rendering, and may be discarded; 
    //     the contents of the attachment will be undefined inside the render area. For attachments with a depth/stencil format, 
    //     this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
    //     For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
    //
    // * initialLayout that the attachment image subresource will be in when a 
    //   render pass instance begins:
    // * finalLayout that the attachment image subresource will be transitioned to 
    //   when a render pass instance ends:
    //
    //   - VK_IMAGE_LAYOUT_UNDEFINED does not support device access.
    //     This layout must only be used as the initialLayout member of VkImageCreateInfo or VkAttachmentDescription, 
    //     or as the oldLayout in an image transition. When transitioning out of this layout, 
    //     the contents of the memory are not guaranteed to be preserved.
    //   - VK_IMAGE_LAYOUT_PREINITIALIZED does not support device access.
    //     This layout must only be used as the initialLayout member of VkImageCreateInfo or VkAttachmentDescription, 
    //     or as the oldLayout in an image transition. When transitioning out of this layout, 
    //     the contents of the memory are preserved. This layout is intended to be used as the initial layout 
    //     for an image whose contents are written by the host, and hence the data can be written to memory immediately, 
    //     without first executing a layout transition.Currently, VK_IMAGE_LAYOUT_PREINITIALIZED is only useful 
    //     with linear images because there is not a standard layout defined for VK_IMAGE_TILING_OPTIMAL images.
    //   - VK_IMAGE_LAYOUT_GENERAL supports all types of device access.
    //   - VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL must only be used as a color or resolve attachment in a VkFramebuffer.
    //     This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT usage bit enabled.
    //   - VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL must only be used as a depth/stencil or depth/stencil resolve attachment 
    //     in a VkFramebuffer. This layout is valid only for image subresources of images created with the 
    //     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT usage bit enabled.
    //   - VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL must only be used as a read-only depth/stencil attachment in a 
    //     VkFramebuffer and /or as a read-only image in a shader (which can be read as a sampled image, 
    //     combined image/sampler and/or input attachment).
    //     This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT 
    //     usage bit enabled. Only image views created with a usage value including VK_IMAGE_USAGE_SAMPLED_BIT 
    //     can be used as a sampled image or combined image/sampler in a shader.
    //     Similarly, only image views created with a usage value including VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT can be used as input attachments.
    //   - VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL: must only be used as a depth/stencil attachment in a VkFramebuffer, 
    //     where the depth aspect is read-only, and/or as a read-only image in a shader (which can be read as a sampled image, 
    //     combined image/sampler and/or input attachment) where only the depth aspect is accessed.
    //     This layout is valid only for image subresources of images created with the 
    //     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT usage bit enabled.
    //     Only image views created with a usage value including VK_IMAGE_USAGE_SAMPLED_BIT 
    //     can be used as a sampled image or combined image/sampler 
    //     in a shader. Similarly, only image views created with a usage value including VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT 
    //     can be used as input attachments.
    //   - VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL: must only be used as a depth/stencil attachment in a VkFramebuffer, 
    //     where the stencil aspect is read only, and/or as a read-only image in a shader (which can be read as a sampled image, 
    //     combined image/sampler and/or input attachment) where only the stencil aspect is accessed. This layout is valid only 
    //     for image subresources of images created with the VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT usage bit enabled.
    //     Only image views created with a usage value including VK_IMAGE_USAGE_SAMPLED_BIT can be used as a sampled image or 
    //     combined image/sampler in a shader. Similarly, only image views created with a usage value including VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT 
    //     can be used as input attachments.
    //   - VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL must only be used as a read-only image in a shader (which can be read as a sampled image, 
    //     combined image/sampler and/or input attachment). This layout is valid only for image subresources of images created with the 
    //     VK_IMAGE_USAGE_SAMPLED_BIT or VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT usage bit enabled.
    //   - VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL must only be used as a source image of a transfer command.
    //     This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_TRANSFER_SRC_BIT usage bit enabled.
    //   - VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL must only be used as a destination image of a transfer command. This layout is valid only 
    //     for image subresources of images created with the VK_IMAGE_USAGE_TRANSFER_DST_BIT usage bit enabled.
    //   - VK_IMAGE_LAYOUT_PRESENT_SRC_KHR must only be used for presenting a presentable image for display.
    //     A swapchain’s image must be transitioned to this layout before calling vkQueuePresentKHR, and must be transitioned away 
    //     from this layout after calling vkAcquireNextImageKHR.
    //   - VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR is valid only for shared presentable images, and must be used for any usage the image supports.
    //   - VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV must only be used as a read - only shading-rate-image.
    //     This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV usage bit enabled.
    //   - VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT must only be used as a fragment density map attachment in a VkRenderPass.
    //     This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT usage bit enabled.
    //
    // Notes:
    // If the attachment uses a color format, then colorAndDepthLoadOperation and colorAndDepthStoreOperation are used, 
    // and stencilLoadOperation stencilStoreOperation are ignored.
    // If the format has depth and/or stencil components, colorAndDepthLoadOperation and colorAndDepthStoreOperation apply only to the depth data, 
    // while stencilLoadOpand stencilStoreOp define how the stencil data is handled. 
    // colorAndDepthLoadOperation and stencilLoadOperation define the load operations that execute as part of the first subpass that uses the attachment.
    // colorAndDepthStoreOperation and stencilStoreOperation define the store operations that execute as part of the last subpass that uses the attachment.
    //
    // The load operation for each sample in an attachment happens-before any recorded command which accesses the sample in the first 
    // subpass where the attachment is used. Load operations for attachments with a depth/stencil format execute in the 
    // VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT pipeline stage. 
    // Load operations for attachments with a color format execute in the VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT pipeline stage.
    //
    // The store operation for each sample in an attachment happens-after any recorded command which accesses the sample in the last 
    // subpass where the attachment is used. 
    // Store operations for attachments with a depth/stencil format execute in the VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT pipeline stage.
    // Store operations for attachments with a color format execute in the VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT pipeline stage.
    //
    // If an attachment is not used by any subpass, then colorAndDepthLoadOperation, colorAndDepthStoreOperation, 
    // stencilLoadOperation, and stencilStoreOperation are ignored, and the attachment’s memory contents will not be modified by execution 
    // of a render pass instance.
    //
    // The load and store operations apply on the first and last use of each view in the render pass, respectively.
    // If a view index of an attachment is not included in the view mask in any subpass that uses it, 
    // then the loadand store operations are ignored, and the attachment’s memory contents will not 
    // be modified by execution of a render pass instance.
    //
    // During a render pass instance, input/color attachments with color formats that have a component size of 8, 16, or 32 bits must be 
    // represented in the attachment’s format throughout the instance. Attachments with other floating-or fixed-point color formats, 
    // or with depth components may be represented in a format with a precision higher than the attachment format, 
    // but must be represented with the same range. When such a component is loaded via the loadOp, it will be converted into an 
    // implementation-dependent format used by the render pass. Such components must be converted from the render pass format, 
    // to the format of the attachment, before they are resolved or stored at the end of a render pass instance via colorAndDepthStoreOperation. 
    //
    // If flags includes VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT, then the attachment is treated as if it shares physical memory 
    // with another attachment in the same render pass. This information limits the ability of the implementation to reorder 
    // certain operations(like layout transitions and the colorAndDepthStoreOperation) such that it is not improperly 
    // reordered against other uses of the same physical memory via a different attachment.
    void 
    add(const VkFormat format,        
        const VkAttachmentLoadOp colorAndDepthLoadOperation,        
        const VkAttachmentStoreOp colorAndDepthStoreOperation,
        const VkImageLayout initialLayout,
        const VkImageLayout finalLayout,
        const VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT, // No multisampling
        const VkAttachmentLoadOp stencilLoadOperation = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        const VkAttachmentStoreOp stencilStoreOperation = VK_ATTACHMENT_STORE_OP_DONT_CARE);

    uint32_t 
    size() const;

    bool
    empty() const;

    const VkAttachmentDescription&  
    operator[](const uint32_t index) const;
       
private:
    std::vector<VkAttachmentDescription> mAttachmentDescriptions = {};
};
}

#endif 