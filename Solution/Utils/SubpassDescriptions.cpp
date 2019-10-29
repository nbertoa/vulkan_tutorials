#include "SubpassDescriptions.h"

#include <cassert>

namespace vk {
void
SubpassDescriptions::add(const VkPipelineBindPoint pipelineBindPoint,
                         const std::vector<VkAttachmentReference>& colorAttachmentReferences,
                         const std::vector<VkAttachmentReference>& inputAttachmentReferences,
                         const std::vector<VkAttachmentReference>& resolveAttachmentReferences,                         
                         const std::vector<VkAttachmentReference>& depthStencilAttachmentReferences,
                         const std::vector<uint32_t>& preserveAttachments) {
    VkSubpassDescription description = {};
    description.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
    description.inputAttachmentCount = static_cast<uint32_t>(inputAttachmentReferences.size());
    description.preserveAttachmentCount = static_cast<uint32_t>(preserveAttachments.size());
    description.pColorAttachments = colorAttachmentReferences.empty() ? nullptr : colorAttachmentReferences.data();
    description.pInputAttachments = inputAttachmentReferences.empty() ? nullptr : inputAttachmentReferences.data();
    description.pDepthStencilAttachment = depthStencilAttachmentReferences.empty() ? nullptr : depthStencilAttachmentReferences.data();
    description.pPreserveAttachments = preserveAttachments.empty() ? nullptr : preserveAttachments.data();
    description.pResolveAttachments = resolveAttachmentReferences.empty() ? nullptr : resolveAttachmentReferences.data();
    mSubpassDescriptions.emplace_back(description);
}

uint32_t
SubpassDescriptions::size() const {
    return static_cast<uint32_t>(mSubpassDescriptions.size());
}

bool
SubpassDescriptions::empty() const {
    return mSubpassDescriptions.empty();
}

const VkSubpassDescription&
SubpassDescriptions::operator[](const uint32_t index) const {
    assert(index < size());

    return mSubpassDescriptions[index];
}

}