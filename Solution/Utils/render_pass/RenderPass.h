#ifndef UTILS_RENDER_PASS_RENDER_PASS
#define UTILS_RENDER_PASS_RENDER_PASS

#include <vector>
#include <vulkan/vulkan.hpp>

#include "AttachmentDescription.h"
#include "SubpassDependency.h"
#include "SubpassDescription.h"

namespace vk2 {
//
// VkRenderPass wrapper.
//
// In other graphics APIs you can take the immediate mode approach and just render
// whatever comes next on your list. This is not possible in Vulkan.
// 
// Instead, you need to plan the rendering of your frame in advance and 
// organize it into passes and subpasses.
//
// A render pass represents a collection of attachments (render targets), subpasses, 
// and dependencies between the subpasses, and describes how the attachments (render targets) 
// are used over the course of the subpasses. 
// The use of a render pass in a command buffer is a render pass instance.
//
// Subpasses are not separate objects and they are important part of 
// the rendering system in Vulkan.
// Fortunately, you do not need to know all the details when preparing your workload.
// For example, you can specify the number of triangles to render on submission.
// The crucial part when defining a RenderPass in Vulkan is the number and 
// formats of attachments that will be used in that pass.
//
// You need the RenderPass to:
// - Create the Pipeline
// - Create the FrameBuffers
// - CommandBuffer execution of the command vkCmdBeginRenderPass
//
class RenderPass {
public:
    // * attachmentDescriptions that are going to be used by the RenderPass:
    //
    // * subpassDescriptions of the RenderPass
    //
    // * subpassDependencies between pairs of subpasses.
    //
    // Notes: The global logical device is the device that creates the render pass.
    RenderPass(const std::vector<AttachmentDescription>& attachmentDescriptions,
               const std::vector<SubpassDescription>& subpassDescriptions,
               const std::vector<SubpassDependency>& subpassDependencies);
    ~RenderPass();
    RenderPass(RenderPass&& other) noexcept;
    RenderPass(const RenderPass&) = delete;
    const RenderPass& operator=(const RenderPass&) = delete;

    VkRenderPass 
    vkRenderPass() const;
    
private:
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
};
}

#endif
