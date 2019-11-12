#ifndef UTILS_RESOURCE_IMAGE
#define UTILS_RESOURCE_IMAGE

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class CommandPool;
class DeviceMemory;
class LogicalDevice;
class PhysicalDevice;

//
// VkImage wrapper
//
// Images represent multidimensional - up to 3 - arrays of data which can be used 
// for various purposes (e.g. attachments, textures), by binding them to a graphics or 
// compute pipeline via descriptor sets, or by directly specifying 
// them as parameters to certain commands.
//
// Buffer and Image are two types of resources that occupy device memory. 
// Buffer is the simpler one. It is a container for any binary data that just has its 
// length, expressed in bytes. 
//
// Image, on the other hand, represents a set of pixels. 
// This is the object known in other graphics APIs as a texture. 
// There are many more parameters needed to specify creation of an Image. 
// It can be 1D, 2D or 3D, have various pixel formats (like R8G8B8A8_UNORM or R32_SFLOAT) 
// and can also consist of many discrete images, because it can have multiple array layers 
// or MIP levels (or both). 
//
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
    // * memoryPropertyFlags is used to create the DeviceMemory:
    //
    //   - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit specifies that memory allocated with 
    //     this type is the most efficient for device access.
    //     This property will be set ifand only if the memory type belongs to a heap 
    //     with the VK_MEMORY_HEAP_DEVICE_LOCAL_BIT set.
    //   - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with 
    //     this type can be mapped for host access using vkMapMemory.
    //   - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache 
    //     management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
    //     are not needed to flush host writes to the device or make device writes visible 
    //     to the host, respectively.
    //   - VK_MEMORY_PROPERTY_HOST_CACHED_BIT bit specifies that memory allocated with this 
    //     type is cached on the host.Host memory accesses to uncached memory are slower than 
    //     to cached memory, however uncached memory is always host coherent.
    //   - VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit specifies that the memory type only 
    //     allows device access to the memory.Memory types must not have both 
    //     VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT and VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set.
    //     Additionally, the object’s backing memory may be provided by the implementation lazily.
    //   - VK_MEMORY_PROPERTY_PROTECTED_BIT bit specifies that the memory type only allows device 
    //     access to the memory, and allows protected queue operations to access the memory.
    //     Memory types must not have VK_MEMORY_PROPERTY_PROTECTED_BIT setand any of 
    //     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set, or VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, 
    //     or VK_MEMORY_PROPERTY_HOST_CACHED_BIT set.
    //   - VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD bit specifies that device accesses to 
    //     allocations of this memory type are automatically made available and visible.
    //   - VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD bit specifies that memory allocated with 
    //     this type is not cached on the device.Uncached device memory is always device coherent.
    //
    // * mipLevelCount of detail available for minified sampling of the image.
    //
    // * initialImageLayout of all image subresources of the image:
    //
    //   - VK_IMAGE_LAYOUT_UNDEFINED does not support device access.
    //     This layout must only be used as the initialLayout member of VkImageCreateInfo or VkAttachmentDescription, 
    //     or as the oldLayout in an image transition.When transitioning out of this layout, 
    //     the contents of the memory are not guaranteed to be preserved.
    //   - VK_IMAGE_LAYOUT_PREINITIALIZED does not support device access.
    //     This layout must only be used as the initialLayout member of VkImageCreateInfo or VkAttachmentDescription, 
    //     or as the oldLayout in an image transition.When transitioning out of this layout, 
    //     the contents of the memory are preserved.This layout is intended to be used as the initial layout for an image 
    //     whose contents are written by the host, and hence the data can be written to memory immediately, 
    //     without first executing a layout transition. Currently, VK_IMAGE_LAYOUT_PREINITIALIZED is only useful with 
    //     linear images because there is not a standard layout defined for VK_IMAGE_TILING_OPTIMAL images.
    //   - VK_IMAGE_LAYOUT_GENERAL supports all types of device access.
    //   - VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL must only be used as a color or resolve attachment in a VkFramebuffer.
    //     This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT usage bit enabled.
    //   - VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL specifies a layout for both the depthand stencil aspects of 
    //     a depth / stencil format image allowing readand write access as a depth / stencil attachment.
    //     It is equivalent to VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHRand VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR.
    //   - VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL specifies a layout for both the depthand stencil aspects of a 
    //     depth / stencil format image allowing read only access as a depth / stencil attachment or in shaders.
    //     It is equivalent to VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHRand VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR.
    //   - VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL specifies a layout for depth / stencil format images 
    //     allowing read and write access to the stencil aspect as a stencil attachment, 
    //     and read only access to the depth aspect as a depth attachment or in shaders.
    //     It is equivalent to VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHRand VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR.
    //   - VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL specifies a layout for depth / stencil format images 
    //     allowing read and write access to the depth aspect as a depth attachment, and read only access to the stencil aspect 
    //     as a stencil attachment or in shaders. It is equivalent to VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR 
    //     and VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR.
    //   - VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR specifies a layout for the depth aspect of a depth / stencil format image 
    //     allowing reada nd write access as a depth attachment.
    //   - VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR specifies a layout for the depth aspect of a depth / stencil format image 
    //     allowing read - only access as a depth attachment or in shaders.
    //   - VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR specifies a layout for the stencil aspect of a depth / stencil format image 
    //     allowing read and write access as a stencil attachment.
    //   - VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR specifies a layout for the stencil aspect of a depth / stencil format image 
    //     allowing read - only access as a stencil attachment or in shaders.
    //   - VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL must only be used as a read - only image in a shader
    //     (which can be read as a sampled image, combined image / sampler and /or input attachment).
    //     This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_SAMPLED_BIT or 
    //     K_IMAGE_USAGE_INPUT_ATTACHMENT_BIT usage bit enabled.
    //   - VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL must only be used as a source image of a transfer command
    //     (see the definition of VK_PIPELINE_STAGE_TRANSFER_BIT).
    //     This layout is valid only for image subresources of images created with 
    //     the VK_IMAGE_USAGE_TRANSFER_SRC_BIT usage bit enabled.
    //   - VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL must only be used as a destination image of a transfer command.
    //     This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_TRANSFER_DST_BIT usage bit enabled.
    //   - VK_IMAGE_LAYOUT_PRESENT_SRC_KHR must only be used for presenting a presentable image for display.
    //     A swapchain’s image must be transitioned to this layout before calling vkQueuePresentKHR, 
    //     and must be transitioned away from this layout after calling vkAcquireNextImageKHR.
    //   - VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR is valid only for shared presentable images, 
    //     and must be used for any usage the image supports.
    //   - VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV must only be used as a read-only shading-rate-image.
    //     This layout is valid only for image subresources of images created with the 
    //     VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV usage bit enabled.
    //   - VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT must only be used as a fragment density map attachment in a VkRenderPass.
    //     This layout is valid only for image subresources of images created with the 
    //     VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT usage bit enabled.
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
    Image(const LogicalDevice& logicalDevice,
          const PhysicalDevice& physicalDevice,   
          const uint32_t imageWidth,
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
    // * physicalDevice to be used to create the staging buffer
    //
    // * transferCommandPool that will be used to create 
    //  the CommandBuffer which will do the transfer operation.
    void
    copyFromDataToDeviceMemory(void* sourceData,
                               const VkDeviceSize size,
                               const PhysicalDevice& physicalDevice,
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

    // Read Buffer() constructor to understand the parameters.
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

    const LogicalDevice& mLogicalDevice;

    VkExtent3D mExtent = {};
    VkImageLayout mLastLayout;

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