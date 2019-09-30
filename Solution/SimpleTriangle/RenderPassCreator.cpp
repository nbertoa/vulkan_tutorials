#include "RenderPassCreator.h"

#include "utils/LogicalDevice.h"
#include "utils/RenderPass.h"
#include "utils/SwapChain.h"

using namespace vk;

RenderPassCreator renderPassCreator() {
    return 
    [](const LogicalDevice& logicalDevice,
       const SwapChain& swapChain) {
           // Create attachment descriptions
           VkAttachmentDescription attachmentDescription = {};
           // The format of the color attachment should match the format 
           // of the swap chain images.
           attachmentDescription.format = swapChain.imageFormat();
           // No multisampling
           attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
           // Determine what to do with the data in the attachment
           // before rendering (load) and after rendering (store)
           // Load:
           // VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents
           // of the attachment.
           // VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant
           // at the start.
           // VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined;
           // we do not care about them.
           //
           // Store:
           // VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored
           // in memory and can be read later.
           // VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer
           // will be undefined after the rendering operation.
           attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
           attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

           // We do not use stencil buffer
           attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
           attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

           // Textures and framebuffers in Vulkan are represented by VkImage objects
           // with a certain pixel format, however the layout of the pixels in 
           // memory can change based on what you are trying to do with an image.
           // The most comman layouts are:
           // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color
           // attachment.
           // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the
           // swap chain.
           // VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as
           // destination for a memory copy operation.
           // 
           // The initial layout specifies which layout the image will have
           // before the render pass begins. 
           // The final layout specifies the layout to automatically transition
           // to wehn the render pass finishes.
           // Using VK_IMAGE_LAYOUT_UNDEFINED for initial layout means
           // that we do not care what previous layout the image was in.
           // We want the image to be ready for presentation using the swap chain 
           // after rendering, which is why we use VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
           // for the final layout.
           attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
           attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

           // Create sub pass descriptions
           VkAttachmentReference attachmentReference = {};
           // Specify which attachment to reference by its index in
           // the attachment descriptions array.
           attachmentReference.attachment = 0;
           // Specify which layout we would like the attachment to have
           // during a subpass that uses this reference. Vulkan will 
           // automatically transition the attachment to this layout
           // when the subpass is started. We want to use the attachment to
           // function as a color buffer.
           attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

           VkSubpassDescription subpassDescription = {};
           // Vulkan may also support a compute subpass in the future, so
           // we have to be axplicit about this being a graphics pipeline.
           subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
           // Specify the refernece to the color attachments. In our case, we have 1.
           subpassDescription.colorAttachmentCount = 1;
           subpassDescription.pColorAttachments = &attachmentReference;

           // Create subpass dependencies
           VkSubpassDependency dependency = {};
           // VK_SUBPASS_EXTERNAL: Implicit subpass before or after the render pass 
           // depending on whether it is specified in srcSubpass or dstSubpass.
           dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
           dependency.dstSubpass = 0; // Our subpass (we have only 1)

           // Specify the operations to wait on and the stages in which these operations occur.
           // We need to wait for the swap chain to finish reading from the image
           // before we can access it.
           // This can be accomplished by waiting on the color attachment output stage itself.
           dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
           dependency.srcAccessMask = 0;

           // The operations that should wait on this are in the color attachment stage
           // adn involve the reading and writing of the color attachment.
           // These settings will prevent the transition from happening until it is
           // actually necessary (and allowed): when we want to start writing colors
           // to it.
           dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
           dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
               VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

           return new RenderPass(logicalDevice,
                                 {attachmentDescription},
                                 {subpassDescription},
                                 {dependency});
    };
}