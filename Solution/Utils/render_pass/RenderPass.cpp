#include "RenderPass.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk2 {
RenderPass::RenderPass(const std::vector<vk::AttachmentDescription>& attachmentDescriptions,
                       const std::vector<vk::SubpassDescription>& subpassDescriptions,
                       const std::vector<vk::SubpassDependency>& subpassDependencies) {
    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    createInfo.pAttachments = attachmentDescriptions.empty() ? 
        nullptr : 
        reinterpret_cast<const VkAttachmentDescription*>(attachmentDescriptions.data());
    createInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
    createInfo.pSubpasses = subpassDescriptions.empty() ? 
        nullptr : 
        reinterpret_cast<const VkSubpassDescription*>(subpassDescriptions.data());
    createInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    createInfo.pDependencies = subpassDependencies.empty() ? 
        nullptr : 
        reinterpret_cast<const VkSubpassDependency*>(subpassDependencies.data());

    vkChecker(vkCreateRenderPass(LogicalDevice::device(),
                                 &createInfo,
                                 nullptr,
                                 &mRenderPass));
    assert(mRenderPass != VK_NULL_HANDLE);
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(LogicalDevice::device(),
                        mRenderPass,
                        nullptr);
}

RenderPass::RenderPass(RenderPass&& other) noexcept
    : mRenderPass(other.mRenderPass)
{
    other.mRenderPass = VK_NULL_HANDLE;
}

VkRenderPass 
RenderPass::vkRenderPass() const {
    assert(mRenderPass != VK_NULL_HANDLE);
    return mRenderPass;
}

}