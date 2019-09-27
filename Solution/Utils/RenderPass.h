#ifndef UTILS_RENDER_PASS
#define UTILS_RENDER_PASS

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkRenderPass wrapper.
// A render pass represents a collection of attachments (render targets), subpasses, 
// and dependencies between the subpasses, and describes how the attachments (render targets) 
// are used over the course of the subpasses. 
// The use of a render pass in a command buffer is a render pass instance.
class RenderPass {
public:
    // VkAttachmentDescription:
    // - flags is a bitmask of VkAttachmentDescriptionFlagBits specifying 
    //   additional properties of the attachment.
    // - format is a VkFormat value specifying the format of the image view that 
    //   will be used for the attachment.
    // - samples is the number of samples of the image as defined in VkSampleCountFlagBits.
    // - loadOp is a VkAttachmentLoadOp value specifying how the contents of color and 
    //   depth components of the attachment are treated at the beginning of the subpass where it is first used.
    // - storeOp is a VkAttachmentStoreOp value specifying how the contents of color and 
    //   depth components of the attachment are treated at the end of the subpass where it is last used.
    // - stencilLoadOp is a VkAttachmentLoadOp value specifying how the contents of stencil 
    //   components of the attachment are treated at the beginning of the subpass where it is first used.
    // - stencilStoreOp is a VkAttachmentStoreOp value specifying how the contents of stencil 
    //   components of the attachment are treated at the end of the last subpass where it is used.
    // - initialLayout is the layout the attachment image subresource will be in when a 
    //   render pass instance begins.
    // - finalLayout is the layout the attachment image subresource will be transitioned to 
    //   when a render pass instance ends.
    //
    // VkSubpassDescription:
    // - flags is a bitmask of VkSubpassDescriptionFlagBits specifying usage of the subpass.
    // - pipelineBindPoint is a VkPipelineBindPoint value specifying the pipeline type 
    //   supported for this subpass.
    // - inputAttachmentCount is the number of input attachments.
    // - pInputAttachments is a pointer to an array of VkAttachmentReference structures 
    //   defining the input attachments for this subpassand their layouts.
    // - colorAttachmentCount is the number of color attachments.
    // - pColorAttachments is a pointer to an array of VkAttachmentReference structures 
    //   defining the color attachments for this subpassand their layouts.
    // - pResolveAttachments is an optional array of colorAttachmentCount VkAttachmentReference 
    //   structures defining the resolve attachments for this subpassand their layouts.
    // - pDepthStencilAttachment is a pointer to a VkAttachmentReference structure specifying 
    //   the depth / stencil attachment for this subpassand its layout.
    // - preserveAttachmentCount is the number of preserved attachments.
    // - pPreserveAttachments is a pointer to an array of preserveAttachmentCount render pass 
    //   attachment indices identifying attachments that are not used by this subpass, 
    //   but whose contents must be preserved throughout the subpass.
    //
    // VkSubpassDependency:
    // - srcSubpass is the subpass index of the first subpass in the dependency, or VK_SUBPASS_EXTERNAL.
    // - dstSubpass is the subpass index of the second subpass in the dependency, or VK_SUBPASS_EXTERNAL.
    // - srcStageMask is a bitmask of VkPipelineStageFlagBits specifying the source stage mask.
    // - dstStageMask is a bitmask of VkPipelineStageFlagBits specifying the destination stage mask
    // - srcAccessMask is a bitmask of VkAccessFlagBits specifying a source access mask.
    // - dstAccessMask is a bitmask of VkAccessFlagBits specifying a destination access mask.
    // - dependencyFlags is a bitmask of VkDependencyFlagBits.
    RenderPass(const LogicalDevice& logicalDevice,
               const std::vector<VkAttachmentDescription>& attachmentDescriptions,
               const std::vector<VkSubpassDescription>& subpassDescriptions,
               const std::vector<VkSubpassDependency>& subpassDependencies);
    ~RenderPass();
    RenderPass(RenderPass&& other) noexcept;

    RenderPass(const RenderPass&) = delete;
    const RenderPass& operator=(const RenderPass&) = delete;

    VkRenderPass vkRenderPass() const;
    
private:
    const LogicalDevice& mLogicalDevice;
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
};
}

#endif
