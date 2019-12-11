#include "ImageMemoryBarrier.h"

#include "Image.h"

namespace vk2 {
ImageMemoryBarrier::ImageMemoryBarrier(const Image& image,
                                       const vk::ImageLayout newImageLayout,
                                       const vk::AccessFlagBits sourceAccessType,
                                       const vk::AccessFlagBits destAccessType,
                                       const VkImageSubresourceRange imageSubresourceRange,
                                       const uint32_t sourceQueueFamilyIndex,
                                       const uint32_t destQueueFamilyIndex) {
    mMemoryBarrier = {};
    mMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    mMemoryBarrier.image = image.vkImage();
    mMemoryBarrier.oldLayout = (VkImageLayout)image.lastImageLayout();
    mMemoryBarrier.newLayout = (VkImageLayout)newImageLayout;
    mMemoryBarrier.srcAccessMask = (VkAccessFlags)sourceAccessType;
    mMemoryBarrier.dstAccessMask = (VkAccessFlags)destAccessType;
    mMemoryBarrier.subresourceRange = imageSubresourceRange;
    mMemoryBarrier.srcQueueFamilyIndex = sourceQueueFamilyIndex;
    mMemoryBarrier.dstQueueFamilyIndex = destQueueFamilyIndex;
}

const VkImageMemoryBarrier&
ImageMemoryBarrier::vkMemoryBarrier() const {
    return mMemoryBarrier;
}
}