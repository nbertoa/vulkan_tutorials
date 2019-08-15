#include "RenderPass.h"

#include <cassert>

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
RenderPass::RenderPass(const LogicalDevice& logicalDevice,
                       const std::vector<VkAttachmentDescription>& attachmentDescriptions,
                       const std::vector<VkSubpassDescription>& subpassDescriptions)
    : mLogicalDevice(logicalDevice)
{
    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    createInfo.pAttachments = attachmentDescriptions.empty() ? nullptr : attachmentDescriptions.data();
    createInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
    createInfo.pSubpasses = subpassDescriptions.empty() ? nullptr : subpassDescriptions.data();

    vkChecker(vkCreateRenderPass(mLogicalDevice.vkDevice(),
                                 &createInfo,
                                 nullptr,
                                 &mRenderPass));
    assert(mRenderPass != VK_NULL_HANDLE);
}

RenderPass::~RenderPass() {
    assert(mRenderPass != VK_NULL_HANDLE);

    vkDestroyRenderPass(mLogicalDevice.vkDevice(),
                        mRenderPass,
                        nullptr);
}

}