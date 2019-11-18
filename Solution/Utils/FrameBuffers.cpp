#include "FrameBuffers.h"

#include <cassert>

#include "DebugUtils.h"
#include "device/LogicalDevice.h"
#include "render_pass/RenderPass.h"

namespace vk {
FrameBuffers::FrameBuffers(const LogicalDevice& logicalDevice,                           
                           const RenderPass& renderPass,
                           const std::vector<VkImageView> imageViews,
                           const uint32_t imageWidth,
                           const uint32_t imageHeight)
    : mLogicalDevice(logicalDevice)
{
    assert(imageViews.empty() == false);
    mFrameBuffers.resize(imageViews.size());
    
    // VkFramebufferCreateInfo:
    // - flags (VK_FRAMEBUFFER_CREATE_):
    //   - IMAGELESS_BIT_KHR, FLAG_BITS_MAX_ENUM 
    // - renderPass is a render pass defining what render passes the framebuffer will be compatible with.
    // - attachmentCount is the number of attachments.
    // - pAttachments is a pointer to an array of VkImageView handles, 
    //   each of which will be used as the corresponding attachment in a render pass instance.
    //   If flags includes VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT_KHR, this parameter is ignored.
    // - width, height and layers define the dimensions of the framebuffer.
    //   If the render pass uses multiview, then layers must be one and each attachment requires a 
    //   number of layers that is greater than the maximum bit index set in the view mask 
    //   in the subpasses in which it is used.
    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = renderPass.vkRenderPass();    
    createInfo.width = imageWidth;
    createInfo.height = imageHeight;
    createInfo.layers = 1;
    
    // Specify the VkImageView objects that should be bound to
    // to the respective attachment descriptions in the render
    // pass attachments array.
    createInfo.attachmentCount = 1;
    for (size_t i = 0; i < imageViews.size(); ++i) {
        assert(imageViews[i] != VK_NULL_HANDLE);
        VkImageView attachments[] = {imageViews[i]};
        createInfo.pAttachments = attachments;

        vkChecker(vkCreateFramebuffer(mLogicalDevice.vkDevice(),
                                      &createInfo,
                                      nullptr,
                                      &mFrameBuffers[i]));
    }
}

FrameBuffers::~FrameBuffers() {
    for (const VkFramebuffer frameBuffer : mFrameBuffers) {
        assert(frameBuffer != VK_NULL_HANDLE);
        vkDestroyFramebuffer(mLogicalDevice.vkDevice(),
                             frameBuffer,
                             nullptr);
    }
}

FrameBuffers::FrameBuffers(FrameBuffers&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mFrameBuffers(std::move(other.mFrameBuffers))
{

}

uint32_t 
FrameBuffers::bufferCount() const {
    assert(mFrameBuffers.empty() == false);
    return static_cast<uint32_t>(mFrameBuffers.size());
}

VkFramebuffer 
FrameBuffers::buffer(const uint32_t bufferIndex) const {
    assert(bufferIndex < mFrameBuffers.size());
    assert(mFrameBuffers[bufferIndex] != VK_NULL_HANDLE);
    return mFrameBuffers[bufferIndex];
}

}