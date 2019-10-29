#include "AttachmentDescriptions.h"

#include <cassert>

namespace vk {
void 
AttachmentDescriptions::add(const VkFormat format,
                            const VkAttachmentLoadOp colorAndDepthLoadOperation,
                            const VkAttachmentStoreOp colorAndDepthStoreOperation,
                            const VkImageLayout initialLayout,
                            const VkImageLayout finalLayout,
                            const VkSampleCountFlagBits sampleCount,
                            const VkAttachmentLoadOp stencilLoadOperation,
                            const VkAttachmentStoreOp stencilStoreOperation) {
    VkAttachmentDescription description = {};
    description.format = format;
    description.samples = sampleCount;
    description.loadOp = colorAndDepthLoadOperation;
    description.stencilLoadOp = stencilLoadOperation;
    description.storeOp = colorAndDepthStoreOperation;
    description.stencilStoreOp = stencilStoreOperation;
    description.initialLayout = initialLayout;
    description.finalLayout = finalLayout;
    mAttachmentDescriptions.emplace_back(description);
}

uint32_t
AttachmentDescriptions::size() const {
    return static_cast<uint32_t>(mAttachmentDescriptions.size());
}

bool
AttachmentDescriptions::empty() const {
    return mAttachmentDescriptions.empty();
}

const VkAttachmentDescription&
AttachmentDescriptions::operator[](const uint32_t index) const {
    assert(index < size());

    return mAttachmentDescriptions[index];
}

}