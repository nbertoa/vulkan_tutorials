#ifndef UTILS_RESOURCE_IMAGE
#define UTILS_RESOURCE_IMAGE

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class CommandPool;
class DeviceMemory;

//
// VkImage wrapper
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
    // * memoryPropertyFlags is used to create the DeviceMemory (VK_MEMORY_PROPERTY_):
    //
    //   - DEVICE_LOCAL_BIT, HOST_VISIBLE_BIT, HOST_COHERENT_BIT, HOST_CACHED_BIT,
    //     LAZILY_ALLOCATED_BIT, PROTECTED_BIT, HOST_VISIBLE_BIT, DEVICE_COHERENT_BIT_AMD,
    //     DEVICE_UNCACHED_BIT_AMD
    //
    // * mipLevelCount of detail available for minified sampling of the image.
    //
    // * initialImageLayout of all image subresources of the image (VK_IMAGE_LAYOUT_)
    //   Images are stored in implementation-dependent opaque layouts in memory.
    //   Each layout has limitations on what kinds of operations are supported for 
    //   image subresources using the layout.
    //   At any given time, the data representing an image subresource in memory exists 
    //   in a particular layout which is determined by the most recent layout transition 
    //   that was performed on that image subresource.Applications have control over which 
    //   layout each image subresource uses, and can transition an image subresource from 
    //   one layout to another. Transitions can happen with an image memory barrier
    //
    //   - UNDEFINED, PREINITIALIZED, GENERAL, COLOR_ATTACHMENT_OPTIMAL, 
    //     DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    //     DEPTH_STENCIL_READ_ONLY_OPTIMAL, DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL, 
    //     DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL, DEPTH_ATTACHMENT_OPTIMAL_KHR, 
    //     DEPTH_READ_ONLY_OPTIMAL_KHR,
    //     STENCIL_ATTACHMENT_OPTIMAL_KHR, STENCIL_READ_ONLY_OPTIMAL_KHR, SHADER_READ_ONLY_OPTIMAL, 
    //     TRANSFER_SRC_OPTIMAL, TRANSFER_DST_OPTIMAL, PRESENT_SRC_KHR, SHARED_PRESENT_KHR, 
    //     SHADING_RATE_OPTIMAL_NV,
    //     SHADING_RATE_IMAGE_BIT_NV, FRAGMENT_DENSITY_MAP_OPTIMAL_EXT
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
          const VkFormat format,
          const VkImageUsageFlags imageUsageFlags,
          const VkMemoryPropertyFlags memoryPropertyFlags,
          const uint32_t mipLevelCount = 1,
          const VkImageLayout initialImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          const VkImageType imageType = VK_IMAGE_TYPE_2D,
          const VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT,
          const uint32_t imageDepth = 1,
          const VkImageTiling imageTiling = VK_IMAGE_TILING_OPTIMAL,
          const uint32_t arrayLayerCount = 1,
          const VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE,
          const VkImageCreateFlags flags = 0,
          const std::vector<uint32_t>& queueFamilyIndices = {});
    ~Image();
    Image(Image&&) noexcept;
    Image(const Image&) = delete;
    const Image& operator=(const Image&) = delete;

    VkImage
    vkImage() const;

    uint32_t 
    width() const;

    uint32_t
    height() const;

    VkImageLayout
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
                               const VkDeviceSize size,
                               const CommandPool& transferCommandPool);

    // * transitionCommandPool that will be used to create 
    //  the CommandBuffer which will do the transition operation.
    void
    transitionImageLayout(const VkImageLayout newImageLayout,
                          const CommandPool& transitionCommandPool);

private:
    // Return the image memory requirements. This is used to create
    // the DeviceMemory (if needed).
    //
    // Memory requirements:
    // - size in bytes, of the memory allocation required for the resource.
    // - alignment in bytes, of the offset within the 
    //   allocation required for the resource.
    // - memoryTypeBits is a bitmask and contains one bit set for every supported 
    //   memory type for the resource. Bit i is set if and only if the memory type i in 
    //   the VkPhysicalDeviceMemoryProperties structure for the physical device 
    //   is supported for the resource.
    VkMemoryRequirements
    imageMemoryRequirements() const;

    // Read Image() constructor to understand the parameters.
    VkImage
    createImage(const VkFormat format,
                const VkImageUsageFlags imageUsageFlags,
                const uint32_t mipLevelCount,
                const VkImageType imageType,
                const VkSampleCountFlagBits sampleCount,
                const VkImageTiling imageTiling,
                const uint32_t arrayLayerCount,
                const VkSharingMode sharingMode,
                const VkImageCreateFlags flags,
                const std::vector<uint32_t>& queueFamilyIndices);
                
    VkExtent3D mExtent = {};
    VkImageLayout mLastLayout;
    VkAccessFlags mLastAccessType = 0;
    VkPipelineStageFlags mLastPipelineStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    VkImage mImage = VK_NULL_HANDLE;    

    // This is only used if the Image is created
    // with the constructor that includes the data needed
    // for DeviceMemory creation.
    // Otherwise, we will use the DeviceMemory provided
    // by the second constructor.
    const bool mHasDeviceMemoryOwnership = true;
    const DeviceMemory* mDeviceMemory = nullptr;
};
}

#endif 