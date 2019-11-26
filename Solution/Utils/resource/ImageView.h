#ifndef UTILS_RESOURCE_IMAGE_VIEW
#define UTILS_RESOURCE_IMAGE_VIEW

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class CommandPool;
class DeviceMemory;
class Image;
class LogicalDevice;
class PhysicalDevice;

//
// VkImageView wrapper
//
// Image objects are not directly accessed by pipeline shaders 
// for reading or writing image data.
// Instead, image views representing contiguous ranges of the 
// image subresources and containing additional metadata are used 
// for that purpose. Views must be created on images of compatible
// types, and must represent a valid subset of image subresources.
//
// You need the ImageView to:
// - Update descriptor sets through vkUpdateDescriptorSets.
//
// To create/use the ImageView you need:
// - Image
//
class ImageView {
public:
    // * format and type used to interpret texel blocks in the image
    //
    // * image on which the view will be created
    //
    // * viewType (VK_IMAGE_VIEW_TYPE_):
    //
    //   - 1D, 2D, 3D, CUBE, 1D_ARRAY, 2D_ARRAY, CUBE_ARRAY
    //
    // * componentMapping for color or depth/stencil.
    //
    // * flags describing additional parameters of the image view.
    ImageView(const LogicalDevice& logicalDevice,
              const VkFormat format,
              const Image& image,
              const VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D,
              const VkComponentMapping& componentMapping = VkComponentMapping{VK_COMPONENT_SWIZZLE_IDENTITY, 
                                                                              VK_COMPONENT_SWIZZLE_IDENTITY, 
                                                                              VK_COMPONENT_SWIZZLE_IDENTITY, 
                                                                              VK_COMPONENT_SWIZZLE_IDENTITY},
              const VkImageViewCreateFlags flags = 0);
    ~ImageView();
    ImageView(ImageView&&) noexcept;
    ImageView(const ImageView&) = delete;
    const ImageView& operator=(const ImageView&) = delete;

    VkImageView
    vkImageView() const;

private:
    // Read ImageView() constructor to understand the parameters.
    VkImageView
    createView(const VkFormat format,
               const Image& image,
               const VkImageViewType viewType,
               const VkComponentMapping& componentsRemapping,
               const VkImageViewCreateFlags flags);

    const LogicalDevice& mLogicalDevice;
    
    VkImageView mImageView = VK_NULL_HANDLE; 
};
}

#endif 