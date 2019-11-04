#include "RenderPass.h"

#include <cassert>

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
RenderPass::RenderPass(const LogicalDevice& logicalDevice,
                       const std::vector<AttachmentDescription>& attachmentDescriptions,
                       const std::vector<SubpassDescription>& subpassDescriptions,
                       const std::vector<SubpassDependency>& subpassDependencies)
    : mLogicalDevice(logicalDevice)
{
    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    createInfo.pAttachments = attachmentDescriptions.empty() ? nullptr : reinterpret_cast<const VkAttachmentDescription*>(attachmentDescriptions.data());
    createInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
    createInfo.pSubpasses = subpassDescriptions.empty() ? nullptr : reinterpret_cast<const VkSubpassDescription*>(subpassDescriptions.data());
    createInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    createInfo.pDependencies = subpassDependencies.empty() ? nullptr : reinterpret_cast<const VkSubpassDependency*>(subpassDependencies.data());

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