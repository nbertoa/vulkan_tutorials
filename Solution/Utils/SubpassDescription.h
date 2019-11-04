#ifndef UTILS_SUBPASS_DESCRIPTION
#define UTILS_SUBPASS_DESCRIPTION

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
//
// VkSubpassDescription wrapper
//
// We need to tell to the render pass the description of its subpasses
//
// You need the SubpassDescription to:
// - Create the RenderPass.
//
class SubpassDescription {
public:
    // * pipelineBindPoint specifying the pipeline type supported for this subpass:
    //   . VK_PIPELINE_BIND_POINT_COMPUTE.
    //   . VK_PIPELINE_BIND_POINT_GRAPHICS.
    //   . VK_PIPELINE_BIND_POINT_RAY_TRACING_NV.
    //
    // * colorAttachmentReferences for this subpass and its layouts:    
    // * inputAttachmentReferences for this subpass and its layouts:    
    // * resolveAttachmentReferences for this subpass and its layouts:
    // * depthStencilAttachmentReferences for this subpass and its layouts:
    //
    //   - attachment is either an integer value identifying an attachment at the corresponding index in 
    //     RenderPass::AttachmentDescriptions, or VK_ATTACHMENT_UNUSED to signify that this attachment is not used.
    //   - layout of the attachment uses during the subpass.
    //
    // * preserveAttachments of render pass 
    //   attachment indices identifying attachments that are not used by this subpass, 
    //   but whose contents must be preserved throughout the subpass.
    //
    // Notes:
    // Each element of the inputAttachmentReferences corresponds to an input attachment index in a fragment shader, 
    // i.e. if a shader declares an image variable decorated with a InputAttachmentIndex value of X, 
    // then it uses the attachment provided in inputAttachmentReferences[X].
    // Input attachments must also be bound to the pipeline in a descriptor set.
    // If the attachment member of any element of inputAttachmentReferences is VK_ATTACHMENT_UNUSED, 
    // the application must not read from the corresponding input attachment index.
    // Fragment shaders can use subpass input variables to access the contents of an input attachment 
    // at the fragment’s(x, y, layer) framebuffer coordinates.
    //
    // Each element of the colorAttachmentReferences corresponds to an output location in the shader, 
    // i.e. if the shader declares an output variable decorated with a Location value of X, 
    // then it uses the attachment provided in colorAttachmentReferences[X].
    // If the attachment member of any element of colorAttachmentReferences is VK_ATTACHMENT_UNUSED, 
    // writes to the corresponding location by a fragment are discarded.
    //
    // If resolveAttachmentReferences is not NULL, each of its elements corresponds to a color attachment 
    // (the element in colorAttachmentReferences at the same index), and a multisample resolve operation is 
    // defined for each attachment. 
    // At the end of each subpass, multisample resolve operations read the subpass’s color attachments, 
    // and resolve the samples for each pixel within the render area to the same pixel location in the 
    // corresponding resolve attachments, unless the resolve attachment index is VK_ATTACHMENT_UNUSED.
    //
    // Similarly, if VkSubpassDescriptionDepthStencilResolveKHR::pDepthStencilResolveAttachment is not NULL 
    // and does not have the value VK_ATTACHMENT_UNUSED, it corresponds to the depth/stencil attachment 
    // in depthStencilAttachmentReferences, and multisample resolve operations for depth and stencil are 
    // defined by VkSubpassDescriptionDepthStencilResolveKHR::depthResolveMode and 
    // VkSubpassDescriptionDepthStencilResolveKHR::stencilResolveMode, respectively.
    // At the end of each subpass, multisample resolve operations read the subpass’s depth/stencil attachment, 
    // and resolve the samples for each pixel to the same pixel location in the corresponding resolve attachment.
    // If VkSubpassDescriptionDepthStencilResolveKHR::depthResolveMode is VK_RESOLVE_MODE_NONE_KHR, 
    // then the depth component of the resolve attachment is not written toand its contents are preserved.
    // Similarly, if VkSubpassDescriptionDepthStencilResolveKHR::stencilResolveMode is VK_RESOLVE_MODE_NONE_KHR, 
    // then the stencil component of the resolve attachment is not written to and its contents are preserved.
    // VkSubpassDescriptionDepthStencilResolveKHR::depthResolveMode is ignored if the VkFormat of the depthStencilAttachmentReferences 
    // does not have a depth component. 
    // Similarly, VkSubpassDescriptionDepthStencilResolveKHR::stencilResolveMode is ignored if the VkFormat of the 
    // depthStencilAttachmentReferences does not have a stencil component.
    //
    // If the image subresource range referenced by the depth/stencil attachment is created with 
    // VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT, 
    // then the multisample resolve operation uses the sample locations state specified in the sampleLocationsInfo 
    // member of the element of the VkRenderPassSampleLocationsBeginInfoEXT::pPostSubpassSampleLocations for the subpass.
    //
    // If depthStencilAttachmentReferences is NULL, or if its attachment index is VK_ATTACHMENT_UNUSED, 
    // it indicates that no depth/stencil attachment will be used in the subpass.
    //
    // The contents of an attachment within the render area become undefined at the start of a 
    // subpass S if all of the following conditions are true:
    // - The attachment is used as a color, depth/stencil, or resolve attachment in any subpass in the render pass.
    // - There is a subpass S1 that uses or preserves the attachment, and a subpass dependency from S1 to S.
    // - The attachment is not used or preserved in subpass S.
    //
    // Once the contents of an attachment become undefined in subpass S, 
    // they remain undefined for subpasses in subpass dependency chains starting with subpass S until they are written again.
    // However, they remain valid for subpasses in other subpass dependency chains starting with subpass S1 
    // if those subpasses use or preserve the attachment.
    SubpassDescription(const VkPipelineBindPoint pipelineBindPoint,
                       const std::vector<VkAttachmentReference>& colorAttachmentReferences,
                       const std::vector<VkAttachmentReference>& inputAttachmentReferences = {},
                       const std::vector<VkAttachmentReference>& resolveAttachmentReferences = {},
                       const std::vector<VkAttachmentReference>& depthStencilAttachmentReferences = {},
                       const std::vector<uint32_t>& preserveAttachments = {});

private:
    VkSubpassDescription mSubpassDescription = {};
};
}

#endif
