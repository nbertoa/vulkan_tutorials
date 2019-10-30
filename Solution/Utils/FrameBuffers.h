#ifndef UTILS_FRAME_BUFFERS
#define UTILS_FRAME_BUFFERS

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;
class RenderPass;

//
// VkFrameBuffers wrapper.
//
// Framebuffer(not to be confused with SwapchainKHR) represents a link to actual Images
// that can be used as attachments (render targets).
// Framebuffers represent a collection of specific memory 
// attachments that a render pass instance uses.
//
// Render passes operate in conjunction with framebuffers.
// You create a Framebuffer object by specifying the RenderPass and a set of ImageViews.
// Of course, their number and formats must match the specification of the RenderPass.
// 
// Framebuffer is another layer on top of Images and basically groups these ImageViews 
// together to be bound as attachments during rendering of a specific RenderPass.
//
// Whenever you begin rendering of a RenderPass, you call the function vkCmdBeginRenderPass
// and you also pass the Framebuffer to it.
//
// You need the FrameBuffers to:
// - CommandBuffer execution of the command vkCmdBeginRenderPass
//
// To create/use the FrameBuffers you need:
// - RenderPass
// - ImageViews
//
class FrameBuffers {
public:
    // * renderPass defining what render passes the framebuffer will be compatible with.
    //
    // * imageViews to be used as the corresponding attachment in a render pass instance.
    //
    // * imageWidth and imageHeight define the dimensions of the framebuffer.
    //
    // Notes:
    // Applications must ensure that all accesses to memory that backs image subresources 
    // used as attachments in a given renderpass instance either happen-before the load operations 
    // for those attachments, or happen-after the store operations for those attachments.
    //
    // For depth/stencil attachments, each aspect can be used separately as attachments and non-attachments 
    // as long as the non-attachment accesses are also via an image subresource in either the 
    // VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL layout or the 
    // VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL layout, 
    // and the attachment resource uses whichever of those two layouts the image accesses do not. 
    // Use of non-attachment aspects in this case is only well defined if the attachment is used in 
    // the subpass where the non-attachment access is being made, or the layout of the image subresource 
    // is constant throughout the entire render pass instance, including the initialLayout and finalLayout.
    //
    // It is legal for a subpass to use no color or depth/stencil attachments, 
    // and rather use shader side effects such as image stores and atomics to produce an output. 
    // In this case, the subpass continues to use the width, height, and layers of the framebuffer 
    // to define the dimensions of the rendering area, and the rasterizationSamples from each pipeline’s 
    // VkPipelineMultisampleStateCreateInfo to define the number of samples used in rasterization; 
    // however, if VkPhysicalDeviceFeatures::variableMultisampleRate is VK_FALSE, 
    // then all pipelines to be bound with a given zero-attachment subpass must have the same value 
    // for VkPipelineMultisampleStateCreateInfo::rasterizationSamples.
    FrameBuffers(const LogicalDevice& logicalDevice,
                 const RenderPass& renderPass,
                 const std::vector<VkImageView> imageViews,
                 const uint32_t imageWidth,
                 const uint32_t imageHeight);
    ~FrameBuffers();
    FrameBuffers(FrameBuffers&& other) noexcept;
    FrameBuffers(const FrameBuffers&) = delete;
    const FrameBuffers& operator=(const FrameBuffers&) = delete;

    uint32_t 
    bufferCount() const;

    VkFramebuffer 
    buffer(const uint32_t bufferIndex) const;

private:
    const LogicalDevice& mLogicalDevice;
    std::vector<VkFramebuffer> mFrameBuffers;
};
}

#endif 