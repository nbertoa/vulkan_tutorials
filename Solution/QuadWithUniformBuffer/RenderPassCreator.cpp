#include "RenderPassCreator.h"

#include "Utils/AttachmentDescriptions.h"
#include "Utils/LogicalDevice.h"
#include "Utils/RenderPass.h"
#include "Utils/SubpassDependencies.h"
#include "Utils/SubpassDescriptions.h"
#include "Utils/SwapChain.h"

using namespace vk;

RenderPassCreator renderPassCreator() {
    return 
    [](const LogicalDevice& logicalDevice,
       const SwapChain& swapChain) {
           AttachmentDescriptions attachmentDescriptions;

           // The format of the color attachment should match the format 
           // of the swap chain images.
           //
           // No multisampling
           //
           // Using VK_IMAGE_LAYOUT_UNDEFINED for initial layout means
           // that we do not care what previous layout the image was in.
           // We want the image to be ready for presentation using the swap chain 
           // after rendering, which is why we use VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
           // for the final layout.
           attachmentDescriptions.add(swapChain.imageFormat(),
                                      VK_ATTACHMENT_LOAD_OP_CLEAR,
                                      VK_ATTACHMENT_STORE_OP_STORE,
                                      VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

           std::vector<VkAttachmentReference> colorAttachmentReferences {{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};
           SubpassDescriptions subpassDescriptions;
           subpassDescriptions.add(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                   colorAttachmentReferences);

           // VK_SUBPASS_EXTERNAL: Implicit subpass before or after the render pass 
           // depending on whether it is specified in srcSubpass or dstSubpass.
           //
           // Specify the operations to wait on and the stages in which these operations occur.
           // We need to wait for the swap chain to finish reading from the image
           // before we can access it.
           // This can be accomplished by waiting on the color attachment output stage itself.
           //
           // The operations that should wait on this are in the color attachment stage
           // adn involve the reading and writing of the color attachment.
           // These settings will prevent the transition from happening until it is
           // actually necessary (and allowed): when we want to start writing colors
           // to it.
           SubpassDependencies subpassDependencies;
           subpassDependencies.add(VK_SUBPASS_EXTERNAL,
                                   0,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   0,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

           return new RenderPass(logicalDevice,
                                 attachmentDescriptions,
                                 subpassDescriptions,
                                 subpassDependencies);
    };
}