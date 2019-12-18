#ifndef UTILS_RESOURCE_IMAGE
#define UTILS_RESOURCE_IMAGE

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vulkan {
//
// Image wrapper
//
// Images (also known as textures) represent multidimensional(up to 3) 
// arrays of data which can be used 
// for various purposes (e.g. attachments, textures), by binding them to a graphics or 
// compute pipeline via descriptor sets, or by directly specifying 
// them as parameters to certain commands.
//
// Buffer and Image are two types of resources that occupy device memory.  
// It can be 1D, 2D or 3D, have various pixel formats (like R8G8B8A8_UNORM or R32_SFLOAT) 
// and can also consist of many discrete images, because it can have multiple array layers 
// or MIP levels (or both). 
// Image is a separate object type because it doesn’t necessarily consist of just a linear 
// set of pixels that can be accessed directly. 
// Images can have a different implementation-specific internal format (tiling and layout) 
// managed by the graphics driver.
//
// Creating an Image with specific dimensions does not automatically allocate memory for it. 
// It is a 3-step process that must be manually performed by you. 
// You can also choose to use our Vulkan Memory Allocator library 
// which takes care of the allocation for you
// - 1. Allocate DeviceMemory,
// - 2. Create Image,
// - 3. Bind them together using function vkBindImageMemory.
//
// That is why you must also create a DeviceMemory object. 
// You should not allocate separate DeviceMemory for each Image. 
// Instead, you should allocate bigger chunks of memory and assign parts of them to your Images. 
// Allocation is a costly operation and there is a limit on maximum number of allocations as well, 
// all of which can be queried from your PhysicalDevice.
//
// One exception to the obligation to allocate and bind DeviceMemory 
// for every Image is the creation of a Swapchain.
//
// You need the Image to:
// - Create the ImageView.
//
// To create/use the Image you need:
// - DeviceMemory for vkBindImageMemory
//
class Image {
public:
    // * imageWidth
    //
    // * imageHeight
    //
    // * format and type of the texel blocks that will be contained in the image.
    //
    // * imageUsageFlags describing the intended usage of the image.
    //
    // * memoryPropertyFlags is used to create the DeviceMemory
    //
    // * mipLevelCount of detail available for minified sampling of the image.
    //
    // * initialImageLayout of all image subresources of the image
    //   Images are stored in implementation-dependent opaque layouts in memory.
    //   Each layout has limitations on what kinds of operations are supported for 
    //   image subresources using the layout.
    //   At any given time, the data representing an image subresource in memory exists 
    //   in a particular layout which is determined by the most recent layout transition 
    //   that was performed on that image subresource.Applications have control over which 
    //   layout each image subresource uses, and can transition an image subresource from 
    //   one layout to another. Transitions can happen with an image memory barrier
    //
    // * imageType specifying the basic dimensionality of the image.
    //   Layers in array textures do not count as a dimension for the purposes of the image type.
    //
    // * sampleCount specifying the number of samples per texel.
    //
    // * imageDepth
    // 
    // * imageTiling specifying the tiling arrangement of the texel blocks in memory.
    //
    // * arrayLayerCount is the number of layers in the image.
    //
    // * sharingMode of the image when it will be accessed by multiple queue families.
    //
    // * flags describing additional parameters of the image.
    //
    // * queueFamilyIndices is a list of queue families that will access this 
    //   image (ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT).
    //
    // Notes: The global logical device owns the image and memory.
    Image(const uint32_t imageWidth,
          const uint32_t imageHeight,
          const vk::Format format,
          const vk::ImageUsageFlags imageUsageFlags,
          const vk::MemoryPropertyFlags deviceMemoryProperties,
          const uint32_t mipLevelCount = 1,
          const vk::ImageLayout initialImageLayout = vk::ImageLayout::eUndefined,
          const vk::ImageType imageType = vk::ImageType::e2D,
          const vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1,
          const uint32_t imageDepth = 1,
          const vk::ImageTiling imageTiling = vk::ImageTiling::eOptimal,
          const uint32_t arrayLayerCount = 1,
          const vk::SharingMode sharingMode = vk::SharingMode::eExclusive,
          const std::vector<uint32_t>& queueFamilyIndices = {});
    ~Image();
    Image(Image&&) noexcept;
    Image(const Image&) = delete;
    const Image& operator=(const Image&) = delete;

    vk::Image
    vkImage() const;

    uint32_t 
    width() const;

    uint32_t
    height() const;

    vk::ImageLayout
    lastImageLayout() const;

    // this method assumes the image was created with
    // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT.
    //
    // This method creates an internal staging buffer to be able to do the copy,
    // and use fences to be signaled once the copy operation finishes.
    //    
    // * transferCommandPool that will be used to create 
    //  the CommandBuffer which will do the transfer operation.
    //
    // Notes: The global physical device is used to create the staging buffer
    void
    copyFromDataToDeviceMemory(void* sourceData,
                               const vk::DeviceSize size,
                               const vk::CommandPool transferCommandPool);

    // * transitionCommandPool that will be used to create 
    //  the CommandBuffer which will do the transition operation.
    void
    transitionImageLayout(const vk::ImageLayout newImageLayout,
                          const vk::CommandPool transitionCommandPool);

private:
    // Read Image() constructor to understand the parameters.
    vk::Image
    createImage(const vk::Format format,
                const vk::ImageUsageFlags imageUsageFlags,
                const uint32_t mipLevelCount,
                const vk::ImageType imageType,
                const vk::SampleCountFlagBits sampleCount,
                const vk::ImageTiling imageTiling,
                const uint32_t arrayLayerCount,
                const vk::SharingMode sharingMode,
                const std::vector<uint32_t>& queueFamilyIndices);
                
    vk::Extent3D mExtent;
    vk::ImageLayout mLastLayout;
    vk::AccessFlagBits mLastAccessType;
    vk::PipelineStageFlagBits mLastPipelineStages = vk::PipelineStageFlagBits::eTopOfPipe;

    vk::Image mImage;    

    // This is only used if the Image is created
    // with the constructor that includes the data needed
    // for DeviceMemory creation.
    // Otherwise, we will use the DeviceMemory provided
    // by the second constructor.
    const bool mHasDeviceMemoryOwnership = true;
    vk::DeviceMemory mDeviceMemory;
};
}

#endif 