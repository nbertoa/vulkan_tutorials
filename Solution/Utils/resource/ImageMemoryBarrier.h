#ifndef UTILS_RESOURCE_IMAGE_MEMORY_BARRIER
#define UTILS_RESOURCE_IMAGE_MEMORY_BARRIER

#include <vulkan/vulkan.h>

namespace vk2 {
class Image;

//
// VkImageMemoryBarrier wrapper
//
// Memory barriers are used to explicitly control access to buffer and image subresource ranges.
// Memory barriers are used to transfer ownership between queue families, change image layouts, 
// and define availability and visibility operations.
// They explicitly define the access types and buffer and image subresource ranges 
// that are included 
// in the access scopes of a memory dependency that is created by a synchronization 
// command that includes them.
//
// Images are stored in implementation-dependent opaque layouts in memory.
// Each layout has limitations on what kinds of operations are supported for image 
// subresources using the layout.
// At any given time, the data representing an image subresource in memory exists in a 
// particular layout 
// which is determined by the most recent layout transition that was performed on that 
// image subresource.
// Applications have control over which layout each image subresource uses, 
// and can transition an image subresource from one layout to another.
// Transitions can happen with an image memory barrier.
//
// Image memory barriers only apply to memory accesses involving a specific image 
// subresource range.
// That is, a memory dependency formed from an image memory barrier is scoped to access via the 
// specified image subresource range.
// Image memory barriers can also be used to define image layout transitions or a queue family 
// ownership transfer for the specified image subresource range.
//
// A pipeline barrier inserts an execution dependency and a set of memory dependencies 
// between a set of commands earlier in the command buffer and a set of commands later 
// in the command buffer.
//
// In Vulkan, we have a new concept called barriers which are called VkImageMemoryBarrier.
// They make sure our operations done on the GPU occur in a particular order which assure 
// we get the expected result .A barrier separates two operations in a queue: before 
// the barrier and after the barrier.
// Work done before the barrier will always finish before it can be used again.
//
// You need the ImageMemoryBarrier to:
// - To execute vkCmdPipelineBarrier in a CommandBuffer.
//
class ImageMemoryBarrier {
public:
    // * image affected by this barrier.
    //
    // * newImageLayout in an image layout transition (VK_IMAGE_LAYOUT_):
    //
    //   - UNDEFINED, PREINITIALIZED, GENERAL, COLOR_ATTACHMENT_OPTIMAL, 
    //     DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    //     DEPTH_STENCIL_READ_ONLY_OPTIMAL, DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL, 
    //     DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL, DEPTH_ATTACHMENT_OPTIMAL_KHR, 
    //     DEPTH_READ_ONLY_OPTIMAL_KHR,
    //     STENCIL_ATTACHMENT_OPTIMAL_KHR, STENCIL_READ_ONLY_OPTIMAL_KHR, SHADER_READ_ONLY_OPTIMAL, 
    //     TRANSFER_SRC_OPTIMAL, TRANSFER_DST_OPTIMAL, PRESENT_SRC_KHR, 
    //     SHARED_PRESENT_KHR, SHADING_RATE_OPTIMAL_NV,
    //     SHADING_RATE_IMAGE_BIT_NV, FRAGMENT_DENSITY_MAP_OPTIMAL_EXT
    //
    // * sourceAccessType and destAccessType
    //   The first and second access scopes are limited to access to 
    //   the memory backing the specified image subresource range (VK_ACCESS_):
    //
    //   - INDIRECT_COMMAND_READ_BIT, INDEX_READ_BIT, VERTEX_ATTRIBUTE_READ_BIT, UNIFORM_READ_BIT,
    //     INPUT_ATTACHMENT_READ_BIT, SHADER_READ_BIT, SHADER_WRITE_BIT, COLOR_ATTACHMENT_READ_BIT,
    //     COLOR_ATTACHMENT_WRITE_BIT, DEPTH_STENCIL_ATTACHMENT_READ_BIT, 
    //     DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    //     TRANSFER_READ_BIT, TRANSFER_WRITE_BIT, HOST_WRITE_BIT, MEMORY_READ_BIT, 
    //     MEMORY_WRITE_BIT,
    //     CONDITIONAL_RENDERING_READ_BIT_EXT, TRANSFORM_FEEDBACK_WRITE_BIT_EXT, 
    //     TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT,
    //     TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT, COMMAND_PROCESS_READ_BIT_NVX, 
    //     COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT,
    //     SHADING_RATE_IMAGE_READ_BIT_NV, ACCELERATION_STRUCTURE_READ_BIT_NV, 
    //     ACCELERATION_STRUCTURE_WRITE_BIT_NV, FRAGMENT_DENSITY_MAP_READ_BIT_EXT
    //
    // Notes:
    // Memory in Vulkan can be accessed from within shader invocations and via some 
    // fixed-function stages of the pipeline. 
    // The access type is a function of the descriptor type used, or how a fixed-function 
    // stage accesses memory. 
    // Each access type corresponds to a bit flag in VkAccessFlagBits.
    // Some synchronization commands take sets of access types as parameters to define the 
    // access scopes of a memory dependency.
    // If a synchronization command includes a source access mask, its first access scope 
    // only includes accesses via the access types specified in that mask.
    // Similarly, if a synchronization command includes a destination access mask, 
    // its second access scope only includes 
    // accesses via the access types specified in that mask.
    //
    // * imageSubresourceRange within image that is affected by this barrier.
    //
    // * sourceQueueFamilyIndex for a queue family ownership transfer.
    //
    // * destQueueFamilyIndex for a queue family ownership transfer.
    ImageMemoryBarrier(const Image& image,
                       const VkImageLayout newImageLayout,
                       const VkAccessFlags sourceAccessType,
                       const VkAccessFlags destAccessType,
                       const VkImageSubresourceRange imageSubresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
                       const uint32_t sourceQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                       const uint32_t destQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED);

    const VkImageMemoryBarrier&
    vkMemoryBarrier() const;

private:
    VkImageMemoryBarrier mMemoryBarrier = {};
};
}

#endif 