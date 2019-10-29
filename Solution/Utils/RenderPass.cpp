#include "RenderPass.h"

#include <cassert>

#include "AttachmentDescriptions.h"
#include "DebugUtils.h"
#include "LogicalDevice.h"
#include "SubpassDependencies.h"
#include "SubpassDescriptions.h"

namespace vk {
RenderPass::RenderPass(const LogicalDevice& logicalDevice,
                       const AttachmentDescriptions& attachmentDescriptions,
                       const SubpassDescriptions& subpassDescriptions,
                       const SubpassDependencies& subpassDependencies)
    : mLogicalDevice(logicalDevice)
{
    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = attachmentDescriptions.size();
    createInfo.pAttachments = attachmentDescriptions.empty() ? nullptr : &attachmentDescriptions[0];
    createInfo.subpassCount = subpassDescriptions.size();
    createInfo.pSubpasses = subpassDescriptions.empty() ? nullptr : &subpassDescriptions[0];
    createInfo.dependencyCount = subpassDependencies.size();
    createInfo.pDependencies = subpassDependencies.empty() ? nullptr : &subpassDependencies[0];

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