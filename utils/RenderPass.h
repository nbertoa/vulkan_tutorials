#ifndef UTILS_RENDER_PASS
#define UTILS_RENDER_PASS

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkRenderPass wrapper to be able to create/destroy/get it easily.
// In Vulkan, a render pass is the set of framebuffer attachments (render targets),
// the way they are used, and the rendering work that is performed using them
class RenderPass {
public:
    RenderPass(const LogicalDevice& logicalDevice,
               const std::vector<VkAttachmentDescription>& attachmentDescriptions,
               const std::vector<VkSubpassDescription>& subpassDescriptions,
               const std::vector<VkSubpassDependency>& subpassDependencies);
    ~RenderPass();
    RenderPass(RenderPass&& other) noexcept;

    RenderPass(const RenderPass&) = delete;
    const RenderPass& operator=(const RenderPass&) = delete;

    VkRenderPass vkRenderPass() const {
        assert(mRenderPass != VK_NULL_HANDLE);
        return mRenderPass;
    }
    
private:
    const LogicalDevice& mLogicalDevice;
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
};
}

#endif
