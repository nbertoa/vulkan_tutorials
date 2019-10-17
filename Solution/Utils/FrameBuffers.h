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
class FrameBuffers {
public:
    // * renderPass is a render pass defining what render passes 
    //   the framebuffer will be compatible with.
    //
    // * imageViews to be used as the corresponding attachment in a render pass instance.
    //
    // * width, height define the dimensions of the framebuffer.
    FrameBuffers(const LogicalDevice& logicalDevice,
                 const RenderPass& renderPass,
                 const std::vector<VkImageView> imageViews,
                 const uint32_t imageWidth,
                 const uint32_t imageHeight);
    ~FrameBuffers();
    FrameBuffers(FrameBuffers&& other) noexcept;
    FrameBuffers(const FrameBuffers&) = delete;
    const FrameBuffers& operator=(const FrameBuffers&) = delete;

    size_t 
    bufferCount() const;

    VkFramebuffer 
    buffer(const size_t bufferIndex) const;

private:
    const LogicalDevice& mLogicalDevice;
    std::vector<VkFramebuffer> mFrameBuffers;
};
}

#endif 