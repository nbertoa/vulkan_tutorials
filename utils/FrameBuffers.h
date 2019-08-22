#ifndef UTILS_FRAME_BUFFERS
#define UTILS_FRAME_BUFFERS

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;
class RenderPass;
class SwapChain;

// VkFrameBuffer's wrapper to be able to create/destroy/handle 
// them easily.
// Render passes operate in conjunction with framebuffers. 
// Framebuffers represent a collection of specific memory 
// attachments that a render pass instance uses.
class FrameBuffers {
public:
    FrameBuffers(const LogicalDevice& logicalDevice,
                 const SwapChain& swapChain,
                 const RenderPass& renderPass);
    ~FrameBuffers();

    FrameBuffers(const FrameBuffers&) = delete;
    const FrameBuffers& operator=(const FrameBuffers&) = delete;

    size_t bufferCount() const {
        assert(mFrameBuffers.empty() == false);
        return mFrameBuffers.size();
    }

    VkFramebuffer buffer(const size_t bufferIndex) const;

private:
    const LogicalDevice& mLogicalDevice;
    std::vector<VkFramebuffer> mFrameBuffers;
};
}

#endif 