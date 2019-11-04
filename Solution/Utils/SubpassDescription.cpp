#include "SubpassDescription.h"

namespace vk {
SubpassDescription::SubpassDescription(const VkPipelineBindPoint pipelineBindPoint,
                                       const std::vector<VkAttachmentReference>& colorAttachmentReferences,
                                       const std::vector<VkAttachmentReference>& inputAttachmentReferences,
                                       const std::vector<VkAttachmentReference>& resolveAttachmentReferences,
                                       const std::vector<VkAttachmentReference>& depthStencilAttachmentReferences,
                                       const std::vector<uint32_t>& preserveAttachments) {
    mSubpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
    mSubpassDescription.inputAttachmentCount = static_cast<uint32_t>(inputAttachmentReferences.size());
    mSubpassDescription.preserveAttachmentCount = static_cast<uint32_t>(preserveAttachments.size());
    mSubpassDescription.pColorAttachments = colorAttachmentReferences.empty() ? nullptr : colorAttachmentReferences.data();
    mSubpassDescription.pInputAttachments = inputAttachmentReferences.empty() ? nullptr : inputAttachmentReferences.data();
    mSubpassDescription.pDepthStencilAttachment = depthStencilAttachmentReferences.empty() ? nullptr : depthStencilAttachmentReferences.data();
    mSubpassDescription.pPreserveAttachments = preserveAttachments.empty() ? nullptr : preserveAttachments.data();
    mSubpassDescription.pResolveAttachments = resolveAttachmentReferences.empty() ? nullptr : resolveAttachmentReferences.data();
}

}