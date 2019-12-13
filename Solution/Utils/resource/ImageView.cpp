#include "ImageView.h"

#include "Image.h"
#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk2 {
ImageView::ImageView(const vk::Format format,
                     const Image& image,
                     const vk::ImageViewType viewType,
                     const vk::ComponentMapping& componentsMapping)
{
    vk::ImageViewCreateInfo info;
    info.setComponents(componentsMapping);
    info.setImage(image.vkImage());
    info.setFormat(format);
    info.setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    info.setViewType(viewType);

    mImageView = LogicalDevice::device().createImageViewUnique(info);
}

ImageView::ImageView(ImageView&& other) noexcept
    : mImageView(std::move(other.mImageView))
{
}

vk::ImageView
ImageView::vkImageView() const {
    assert(mImageView.get() != VK_NULL_HANDLE);
    return mImageView.get();
}

}