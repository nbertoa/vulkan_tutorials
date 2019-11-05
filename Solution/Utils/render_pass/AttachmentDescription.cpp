#include "AttachmentDescription.h"

namespace vk {
AttachmentDescription::AttachmentDescription(const VkFormat format,
                                             const VkAttachmentLoadOp colorAndDepthLoadOperation,
                                             const VkAttachmentStoreOp colorAndDepthStoreOperation,
                                             const VkImageLayout initialLayout,
                                             const VkImageLayout finalLayout,
                                             const VkSampleCountFlagBits sampleCount,
                                             const VkAttachmentLoadOp stencilLoadOperation,
                                             const VkAttachmentStoreOp stencilStoreOperation) {
    mAttachmentDescription.format = format;
    mAttachmentDescription.samples = sampleCount;
    mAttachmentDescription.loadOp = colorAndDepthLoadOperation;
    mAttachmentDescription.stencilLoadOp = stencilLoadOperation;
    mAttachmentDescription.storeOp = colorAndDepthStoreOperation;
    mAttachmentDescription.stencilStoreOp = stencilStoreOperation;
    mAttachmentDescription.initialLayout = initialLayout;
    mAttachmentDescription.finalLayout = finalLayout;
}

}