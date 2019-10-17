#include "RenderPass.h"

#include <cassert>

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
RenderPass::RenderPass(const LogicalDevice& logicalDevice,
                       const std::vector<VkAttachmentDescription>& attachmentDescriptions,
                       const std::vector<VkSubpassDescription>& subpassDescriptions,
                       const std::vector<VkSubpassDependency>& subpassDependencies)
    : mLogicalDevice(logicalDevice)
{
    // VkRenderPassCreateInfo:
    // - attachmentCount is the number of attachments used by this render pass.
    // - pAttachments is a pointer to an array of attachmentCount VkAttachmentDescription structures 
    //   describing the attachments used by the render pass.
    // - subpassCount is the number of subpasses to create.
    // - pSubpasses is a pointer to an array of subpassCount VkSubpassDescription structures describing each subpass.
    // - dependencyCount is the number of memory dependencies between pairs of subpasses.
    // - pDependencies is a pointer to an array of dependencyCount VkSubpassDependency structures 
    //   describing dependencies between pairs of subpasses.
    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    createInfo.pAttachments = attachmentDescriptions.empty() ? nullptr : attachmentDescriptions.data();
    createInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
    createInfo.pSubpasses = subpassDescriptions.empty() ? nullptr : subpassDescriptions.data();
    createInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    createInfo.pDependencies = subpassDependencies.empty() ? nullptr : subpassDependencies.data();

    vkChecker(vkCreateRenderPass(mLogicalDevice.vkDevice(),
                                 &createInfo,
                                 nullptr,
                                 &mRenderPass));
    assert(mRenderPass != VK_NULL_HANDLE);
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(mLogicalDevice.vkDevice(),
                        mRenderPass,
                        nullptr);
}

RenderPass::RenderPass(RenderPass&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mRenderPass(other.mRenderPass)
{
    other.mRenderPass = VK_NULL_HANDLE;
}

VkRenderPass 
RenderPass::vkRenderPass() const {
    assert(mRenderPass != VK_NULL_HANDLE);
    return mRenderPass;
}

}