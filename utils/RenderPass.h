#ifndef UTILS_RENDER_PASS
#define UTILS_RENDER_PASS

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
               const std::vector<VkSubpassDescription>& subpassDescriptions);
    ~RenderPass();
    
private:
    const LogicalDevice& mLogicalDevice;
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
};
}

#endif
