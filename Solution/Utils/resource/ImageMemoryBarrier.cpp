#include "ImageMemoryBarrier.h"

#include "Image.h"

namespace vk {
ImageMemoryBarrier::ImageMemoryBarrier(const Image& image,
                                       const VkImageLayout newImageLayout,
                                       const VkAccessFlags sourceAccessType,
                                       const VkAccessFlags destAccessType,
                                       const VkImageSubresourceRange imageSubresourceRange,
                                       const uint32_t sourceQueueFamilyIndex,
                                       const uint32_t destQueueFamilyIndex) {
    mMemoryBarrier = {};
    mMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    mMemoryBarrier.image = image.vkImage();
    mMemoryBarrier.oldLayout = image.lastImageLayout();
    mMemoryBarrier.newLayout = newImageLayout;
    mMemoryBarrier.srcAccessMask = sourceAccessType;
    mMemoryBarrier.dstAccessMask = destAccessType;
    mMemoryBarrier.subresourceRange = imageSubresourceRange;
    mMemoryBarrier.srcQueueFamilyIndex = sourceQueueFamilyIndex;
    mMemoryBarrier.dstQueueFamilyIndex = destQueueFamilyIndex;
}

const VkImageMemoryBarrier&
ImageMemoryBarrier::vkMemoryBarrier() const {
    return mMemoryBarrier;
}
}