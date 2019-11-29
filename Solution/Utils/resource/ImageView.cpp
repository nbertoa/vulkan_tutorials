#include "ImageView.h"

#include "Image.h"
#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk2 {
ImageView::ImageView(const VkFormat format,
                     const Image& image,
                     const VkImageViewType viewType,
                     const VkComponentMapping& componentsMapping,
                     const VkImageViewCreateFlags flags)
{
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image.vkImage();
    createInfo.format = format;
    createInfo.flags = flags;
    createInfo.components = componentsMapping;
    createInfo.viewType = viewType;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    vkChecker(vkCreateImageView(LogicalDevice::vkDevice(),
                                &createInfo,
                                nullptr,
                                &mImageView));
}

ImageView::~ImageView() {
    vkDestroyImageView(LogicalDevice::vkDevice(),
                       mImageView,
                       nullptr);
}

ImageView::ImageView(ImageView&& other) noexcept
    : mImageView(other.mImageView)
{
    other.mImageView = VK_NULL_HANDLE;
}

VkImageView
ImageView::vkImageView() const {
    assert(mImageView != VK_NULL_HANDLE);
    return mImageView;
}

}