#include "ImageView.h"

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"
#include "Image.h"

namespace vk {
ImageView::ImageView(const LogicalDevice& logicalDevice,
                     const VkFormat format,
                     const Image& image,
                     const VkImageViewType type,
                     const VkComponentMapping& componentsMapping,
                     const VkImageViewCreateFlags flags)
    : mLogicalDevice(logicalDevice)
    , mImageView(createView(format,
                            image,
                            type,
                            componentsMapping,
                            flags))
{
}

ImageView::~ImageView() {
    vkDestroyImageView(mLogicalDevice.vkDevice(),
                       mImageView,
                       nullptr);
}

ImageView::ImageView(ImageView&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mImageView(other.mImageView)
{
    other.mImageView = VK_NULL_HANDLE;
}

VkImageView
ImageView::vkImageView() const {
    assert(mImageView != VK_NULL_HANDLE);
    return mImageView;
}

VkImageView
ImageView::createView(const VkFormat format,
                      const Image& image,
                      const VkImageViewType viewType,
                      const VkComponentMapping& componentsMapping,
                      const VkImageViewCreateFlags flags) {
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

    VkImageView imageView;
    vkChecker(vkCreateImageView(mLogicalDevice.vkDevice(),
                                &createInfo,
                                nullptr,
                                &imageView));

    return imageView;
}

}