#include "FrameBuffers.h"

#include <cassert>

#include "DebugUtils.h"
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "SwapChain.h"

namespace vk {
FrameBuffers::FrameBuffers(const LogicalDevice& logicalDevice,
                           const SwapChain& swapChain,
                           const RenderPass& renderPass) 
    : mLogicalDevice(logicalDevice)
{
    const std::vector<VkImageView> imageViews = swapChain.imageViews();
    mFrameBuffers.resize(imageViews.size());
    
    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    // Specify witch which render pass the framebuffer needs 
    // to be compatible.
    createInfo.renderPass = renderPass.vkRenderPass();    
    createInfo.width = swapChain.imageWidth();
    createInfo.height = swapChain.imageHeight();
    // The number of layers in image arrays (our swap chain
    // images are single images)
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

size_t 
FrameBuffers::bufferCount() const {
    assert(mFrameBuffers.empty() == false);
    return mFrameBuffers.size();
}

VkFramebuffer 
FrameBuffers::buffer(const size_t bufferIndex) const {
    assert(bufferIndex < mFrameBuffers.size());
    assert(mFrameBuffers[bufferIndex] != VK_NULL_HANDLE);
    return mFrameBuffers[bufferIndex];
}

}