#ifndef UTILS_RESOURCE_IMAGE_MEMORY_BARRIER
#define UTILS_RESOURCE_IMAGE_MEMORY_BARRIER

#include <vulkan/vulkan.h>

namespace vk {
class Image;

//
// VkImageMemoryBarrier wrapper
//
// A pipeline barrier inserts an execution dependency and a set of memory dependencies between a set of 
// commands earlier in the command buffer and a set of commands later in the command buffer.
//
// In Vulkan, we have a new concept called barriers which are called VkImageMemoryBarrier.
// They make sure our operations done on the GPU occur in a particular order which assure 
// we get the expected result.A barrier separates two operations in a queue: before the barrier and after the barrier.
// Work done before the barrier will always finish before it can be used again.
//
// You need the ImageMemoryBarrier to:
// - To execute vkCmdPipelineBarrier in a CommandBuffer.
//
class ImageMemoryBarrier {
public:
    // * image affected by this barrier.
    //
    // * newImageLayout in an image layout transition:
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
    //   - VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV must only be used as a read - only shading - rate - image.
    //     This layout is valid only for image subresources of images created with the 
    //     VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV usage bit enabled.
    //   - VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT must only be used as a fragment density map attachment in a VkRenderPass.
    //     This layout is valid only for image subresources of images created with the 
    //     VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT usage bit enabled.
    //
    // * sourceAccessMask specifying a source access mask.
    // * destAccessMask specifying a destination access mask.
    //
    //   - VK_ACCESS_INDIRECT_COMMAND_READ_BIT specifies read access to indirect command data 
    //     read as part of an indirect drawing or dispatch command.
    //   - VK_ACCESS_INDEX_READ_BIT specifies read access to an index buffer as part of an 
    //     indexed drawing command, bound by vkCmdBindIndexBuffer.
    //   - VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT specifies read access to a vertex buffer as part 
    //     of a drawing command, bound by vkCmdBindVertexBuffers.
    //   - VK_ACCESS_UNIFORM_READ_BIT specifies read access to a uniform buffer.
    //   - VK_ACCESS_INPUT_ATTACHMENT_READ_BIT specifies read access to an input attachment 
    //     within a render pass during fragment shading.
    //   - VK_ACCESS_SHADER_READ_BIT specifies read access to a storage buffer, physical storage buffer, 
    //     uniform texel buffer, storage texel buffer, sampled image, or storage image.
    //   - VK_ACCESS_SHADER_WRITE_BIT specifies write access to a storage buffer, physical storage buffer, 
    //     storage texel buffer, or storage image.
    //   - VK_ACCESS_COLOR_ATTACHMENT_READ_BIT specifies read access to a color attachment, such as via blending, 
    //     logic operations, or via certain subpass load operations.It does not include advanced blend operations.
    //   - VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT specifies write access to a color, resolve, 
    //     or depth/stencil resolve attachment during a render pass or via certain subpass loadand store operations.
    //   - VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT specifies read access to a depth/stencil attachment, 
    //     via depth or stencil operations or via certain subpass load operations.
    //   - VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT specifies write access to a depth/stencil attachment, 
    //     via depth or stencil operations or via certain subpass loadand store operations.
    //   - VK_ACCESS_TRANSFER_READ_BIT specifies read access to an image or buffer in a copy operation.
    //   - VK_ACCESS_TRANSFER_WRITE_BIT specifies write access to an image or buffer in a clear or copy operation.
    //   - VK_ACCESS_HOST_READ_BIT specifies read access by a host operation.Accesses of 
    //     this type are not performed through a resource, but directly on memory.
    //   - VK_ACCESS_HOST_WRITE_BIT specifies write access by a host operation.
    //     Accesses of this type are not performed through a resource, but directly on memory.
    //   - VK_ACCESS_MEMORY_READ_BIT specifies all read accesses.It is always valid in any access mask, 
    //     and is treated as equivalent to setting all READ access flags that are valid where it is used.
    //   - VK_ACCESS_MEMORY_WRITE_BIT specifies all write accesses.It is always valid in any access mask, 
    //     and is treated as equivalent to setting all WRITE access flags that are valid where it is used.
    //   - VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT specifies read access to a predicate 
    //     as part of conditional rendering.
    //   - VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT specifies write access to a transform feedback 
    //     buffer made when transform feedback is active.
    //   - VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT specifies read access to a transform feedback 
    //     counter buffer which is read when vkCmdBeginTransformFeedbackEXT executes.
    //   - VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT specifies write access to a transform feedback 
    //     counter buffer which is written when vkCmdEndTransformFeedbackEXT executes.
    //   - VK_ACCESS_COMMAND_PROCESS_READ_BIT_NVX specifies reads from VkBuffer inputs to vkCmdProcessCommandsNVX.
    //   - VK_ACCESS_COMMAND_PROCESS_WRITE_BIT_NVX specifies writes to the target 
    //     command buffer in vkCmdProcessCommandsNVX.
    //   - VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT is similar to VK_ACCESS_COLOR_ATTACHMENT_READ_BIT, 
    //     but also includes advanced blend operations.
    //   - VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV specifies read access to a shading rate image as part 
    //     of a drawing command, as bound by vkCmdBindShadingRateImageNV.
    //   - VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV specifies read access to an acceleration structure as 
    //     part of a trace or build command.
    //   - VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV specifies write access to an acceleration structure 
    //     as part of a build command.
    //   - VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT specifies read access to a fragment density map attachment 
    //     during dynamic fragment density map operations
    //
    // * imageSubresourceRange within image that is affected by this barrier.
    //
    // * sourceQueueFamilyIndex for a queue family ownership transfer.
    //
    // * destQueueFamilyIndex for a queue family ownership transfer.
    ImageMemoryBarrier(const Image& image,
                       const VkImageLayout newImageLayout,
                       const VkAccessFlags sourceAccessMask,
                       const VkAccessFlags destAccessMask,
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