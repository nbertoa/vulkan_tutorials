#include "FrameBuffers.h"

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
    createInfo.renderPass = renderPass.vkRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.width = swapChain.imageWidth();
    createInfo.height = swapChain.imageHeight();
    createInfo.layers = 1;
    
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
    assert(mFrameBuffers.empty() == false);

    for (const VkFramebuffer frameBuffer : mFrameBuffers) {
        assert(frameBuffer != VK_NULL_HANDLE);
        vkDestroyFramebuffer(mLogicalDevice.vkDevice(),
                             frameBuffer,
                             nullptr);
    }
}

VkFramebuffer 
FrameBuffers::buffer(const size_t bufferIndex) const {
    assert(bufferIndex < mFrameBuffers.size());
    assert(mFrameBuffers[bufferIndex] != VK_NULL_HANDLE);
    return mFrameBuffers[bufferIndex];
}

}