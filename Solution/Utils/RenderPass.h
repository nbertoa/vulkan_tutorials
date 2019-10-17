#ifndef UTILS_RENDER_PASS
#define UTILS_RENDER_PASS

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

//
// VkRenderPass wrapper.
//
// In other graphics APIs you can take the immediate mode approach and just render
// whatever comes next on your list. This is not possible in Vulkan.
// 
// Instead, you need to plan the rendering of your frame in advance and 
// organize it into passes and subpasses.
//
// A render pass represents a collection of attachments (render targets), subpasses, 
// and dependencies between the subpasses, and describes how the attachments (render targets) 
// are used over the course of the subpasses. 
// The use of a render pass in a command buffer is a render pass instance.
//
// Subpasses are not separate objects and they are important part of the rendering system in Vulkan.
// Fortunately, you do not need to know all the details when preparing your workload.
// For example, you can specify the number of triangles to render on submission.
// The crucial part when defining a RenderPass in Vulkan is the number and 
// formats of attachments that will be used in that pass.
//
// You need the RenderPass to:
// - Create the Pipeline
// - Create the FrameBuffers
// - CommandBuffer execution of the command vkCmdBeginRenderPass
//
class RenderPass {
public:
    // * attachmentDescriptions are the descriptions of the attachments that
    //   are going to be used by the RenderPass:
    //
    //   - flags is a bitmask of VkAttachmentDescriptionFlagBits specifying 
    //     additional properties of the attachment.
    //     - VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT specifies that the attachment 
    //       aliases the same device memory as other attachments.
    //     - VK_ATTACHMENT_DESCRIPTION_FLAG_BITS_MAX_ENUM 
    //   - format is a VkFormat value specifying the format of the image view that 
    //     will be used for the attachment.
    //   - samples is the number of samples of the image as defined in VkSampleCountFlagBits.
    //   - loadOp is a VkAttachmentLoadOp value specifying how the contents of color and 
    //     depth components of the attachment are treated at the beginning of the subpass where it is first used.
    //   - stencilLoadOp is a VkAttachmentLoadOp value specifying how the contents of stencil 
    //     components of the attachment are treated at the beginning of the subpass where it is first used.
    //
    //     - VK_ATTACHMENT_LOAD_OP_LOAD specifies that the previous contents of the image within the render area will be preserved. 
    //       For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT. 
    //       For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_READ_BIT.
    //     - VK_ATTACHMENT_LOAD_OP_CLEAR specifies that the contents within the render area will be cleared to a uniform value, 
    //       which is specified when a render pass instance is begun.
    //       For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
    //       For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
    //     - VK_ATTACHMENT_LOAD_OP_DONT_CARE specifies that the previous contents within the area need not be preserved; 
    //       the contents of the attachment will be undefined inside the render area.
    //       For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
    //       For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
    //
    //   - storeOp is a VkAttachmentStoreOp value specifying how the contents of color and 
    //     depth components of the attachment are treated at the end of the subpass where it is last used.
    //   - stencilStoreOp is a VkAttachmentStoreOp value specifying how the contents of stencil 
    //     components of the attachment are treated at the end of the last subpass where it is used.
    //
    //     VkAttachmentStoreOp:
    //    - VK_ATTACHMENT_STORE_OP_STORE specifies the contents generated during the render pass and within the render area 
    //       are written to memory. For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
    //       For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
    //     - VK_ATTACHMENT_STORE_OP_DONT_CARE specifies the contents within the render area are not needed after rendering, and may be discarded; 
    //       the contents of the attachment will be undefined inside the render area. For attachments with a depth/stencil format, 
    //       this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
    //       For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
    //
    //   - initialLayout is the layout the attachment image subresource will be in when a 
    //     render pass instance begins.
    //   - finalLayout is the layout the attachment image subresource will be transitioned to 
    //     when a render pass instance ends.
    //
    // * subpassDescriptions descriptions of each subpass of the RenderPass:
    //
    //   - flags is a bitmask of VkSubpassDescriptionFlagBits specifying usage of the subpass.
    //   - pipelineBindPoint is a VkPipelineBindPoint value specifying the pipeline type 
    //     supported for this subpass.
    //     - VK_PIPELINE_BIND_POINT_COMPUTE specifies binding as a compute pipeline.
    //     - VK_PIPELINE_BIND_POINT_GRAPHICS specifies binding as a graphics pipeline.
    //     - VK_PIPELINE_BIND_POINT_RAY_TRACING_NV specifies binding as a ray tracing pipeline.
    //   - inputAttachmentCount is the number of input attachments.
    //   - colorAttachmentCount is the number of color attachments.
    //   - pInputAttachments is a pointer to an array of VkAttachmentReference structures 
    //     defining the input attachments for this subpassand their layouts.
    //   - pColorAttachments is a pointer to an array of VkAttachmentReference structures 
    //     defining the color attachments for this subpassand their layouts.
    //   - pResolveAttachments is an optional array of colorAttachmentCount VkAttachmentReference 
    //     structures defining the resolve attachments for this subpassand their layouts.
    //   - pDepthStencilAttachment is a pointer to a VkAttachmentReference structure specifying 
    //     the depth / stencil attachment for this subpassand its layout.
    //
    //     VkAttachmentReference:
    //     - attachment is either an integer value identifying an attachment at the corresponding index in 
    //       VkRenderPassCreateInfo::pAttachments, or VK_ATTACHMENT_UNUSED to signify that this attachment is not used.
    //     - layout is a VkImageLayout value specifying the layout the attachment uses during the subpass.
    //
    //   - preserveAttachmentCount is the number of preserved attachments.
    //   - pPreserveAttachments is a pointer to an array of preserveAttachmentCount render pass 
    //     attachment indices identifying attachments that are not used by this subpass, 
    //     but whose contents must be preserved throughout the subpass.
    //
    // * subpassDependencies describes dependencies between pairs of subpasses
    //
    //   - srcSubpass is the subpass index of the first subpass in the dependency, or VK_SUBPASS_EXTERNAL.
    //   - dstSubpass is the subpass index of the second subpass in the dependency, or VK_SUBPASS_EXTERNAL.
    //   - srcStageMask is a bitmask of VkPipelineStageFlagBits specifying the source stage mask.
    //   - dstStageMask is a bitmask of VkPipelineStageFlagBits specifying the destination stage mask
    //     VkPipelineStageFlagBits:
    //     - VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT specifies the stage of the pipeline 
    //       where any commands are initially received by the queue.
    //     - VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT specifies the stage of the pipeline where 
    //       Draw/DispatchIndirect data structures are consumed.This stage also includes 
    //       reading commands written by vkCmdProcessCommandsNVX.
    //     - VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV specifies the task shader stage.
    //     - VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV specifies the mesh shader stage.
    //     - VK_PIPELINE_STAGE_VERTEX_INPUT_BIT specifies the stage of the pipeline where 
    //       vertex and index buffers are consumed.
    //     - VK_PIPELINE_STAGE_VERTEX_SHADER_BIT specifies the vertex shader stage.
    //     - VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT specifies the tessellation 
    //       control shader stage.
    //     - VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT specifies the tessellation 
    //       evaluation shader stage.
    //     - VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT specifies the geometry shader stage.
    //     - VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT specifies the fragment shader stage.
    //     - VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT specifies the stage of the pipeline where 
    //       early fragment tests(depth and stencil tests before fragment shading) are performed.
    //       This stage also includes subpass load operations for framebuffer attachments with a depth/stencil format.
    //     - VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT specifies the stage of the pipeline where late 
    //       fragment tests(depth and stencil tests after fragment shading) are performed.
    //       This stage also includes subpass store operations for framebuffer attachments with a depth/stencil format.
    //     - VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT specifies the stage of the pipeline after 
    //       blending where the final color values are output from the pipeline.This stage also includes 
    //       subpass load and store operationsand multisample resolve operations for framebuffer 
    //       attachments with a color or depth / stencil format.
    //     - VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT specifies the execution of a compute shader.
    //     - VK_PIPELINE_STAGE_TRANSFER_BIT specifies the execution of copy commands.
    //       This includes the operations resulting from all copy commands, clear commands(with the exception of vkCmdClearAttachments), 
    //       and vkCmdCopyQueryPoolResults.
    //     - VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT specifies the final stage in the pipeline where operations 
    //       generated by all commands complete execution.
    //     - VK_PIPELINE_STAGE_HOST_BIT specifies a pseudo-stage indicating execution on the host of reads/writes 
    //       of device memory. This stage is not invoked by any commands recorded in a command buffer.
    //     - VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV specifies the execution of the ray tracing shader stages.
    //     - VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV specifies the execution of vkCmdBuildAccelerationStructureNV, 
    //       vkCmdCopyAccelerationStructureNV, and vkCmdWriteAccelerationStructuresPropertiesNV.
    //     - VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT specifies the execution of all graphics pipeline stages
    //     - VK_PIPELINE_STAGE_ALL_COMMANDS_BIT is equivalent to the logical OR of every other pipeline stage flag that 
    //       is supported on the queue it is used with.
    //     - VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT specifies the stage of the pipeline where the predicate of 
    //       conditional rendering is consumed.
    //     - VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT specifies the stage of the pipeline where vertex attribute 
    //       output values are written to the transform feedback buffers.
    //     - VK_PIPELINE_STAGE_COMMAND_PROCESS_BIT_NVX specifies the stage of the pipeline where device-side generation of 
    //       commands via vkCmdProcessCommandsNVX is handled.
    //     - VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV specifies the stage of the pipeline where the shading rate image 
    //       is read to determine the shading rate for portions of a rasterized primitive.
    //     - VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT specifies the stage of the pipeline where the fragment 
    //       density map is read to generate the fragment areas.
    //
    //   - srcAccessMask is a bitmask of VkAccessFlagBits specifying a source access mask.
    //   - dstAccessMask is a bitmask of VkAccessFlagBits specifying a destination access mask.
    //       
    //     VkAccessFlagBits:
    //     - VK_ACCESS_INDIRECT_COMMAND_READ_BIT specifies read access to indirect command data read as part 
    //       of an indirect drawing or dispatch command.
    //     - VK_ACCESS_INDEX_READ_BIT specifies read access to an index buffer as part of an indexed drawing command, 
    //       bound by vkCmdBindIndexBuffer.
    //     - VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT specifies read access to a vertex buffer as part of a drawing command, 
    //       bound by vkCmdBindVertexBuffers.
    //     - VK_ACCESS_UNIFORM_READ_BIT specifies read access to a uniform buffer.
    //     - VK_ACCESS_INPUT_ATTACHMENT_READ_BIT specifies read access to an input attachment within 
    //       a render pass during fragment shading.
    //     - VK_ACCESS_SHADER_READ_BIT specifies read access to a storage buffer, physical storage buffer, 
    //       uniform texel buffer, storage texel buffer, sampled image, or storage image.
    //     - VK_ACCESS_SHADER_WRITE_BIT specifies write access to a storage buffer, physical storage buffer, 
    //       storage texel buffer, or storage image.
    //     - VK_ACCESS_COLOR_ATTACHMENT_READ_BIT specifies read access to a color attachment, such as via blending, 
    //       logic operations, or via certain subpass load operations.It does not include advanced blend operations.
    //     - VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT specifies write access to a color, resolve, or depth/stencil 
    //       resolve attachment during a render pass or via certain subpass loadand store operations.
    //     - VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT specifies read access to a depth / stencil attachment, 
    //       via depth or stencil operations or via certain subpass load operations.
    //     - VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT specifies write access to a depth / stencil attachment, 
    //       via depth or stencil operations or via certain subpass loadand store operations.
    //     - VK_ACCESS_TRANSFER_READ_BIT specifies read access to an image or buffer in a copy operation.
    //     - VK_ACCESS_TRANSFER_WRITE_BIT specifies write access to an image or buffer in a clear or copy operation.
    //     - VK_ACCESS_HOST_READ_BIT specifies read access by a host operation.Accesses of this type are not performed 
    //       through a resource, but directly on memory.
    //     - VK_ACCESS_HOST_WRITE_BIT specifies write access by a host operation.Accesses of this type are not performed 
    //       through a resource, but directly on memory.
    //     - VK_ACCESS_MEMORY_READ_BIT specifies read access via non - specific entities.These entities include the Vulkan 
    //       device and host, but may also include entities external to the Vulkan device or otherwise not part of the core Vulkan pipeline.
    //       When included in a destination access mask, makes all available writes visible to all future read accesses on entities known to the Vulkan device.
    //     - VK_ACCESS_MEMORY_WRITE_BIT specifies write access via non - specific entities.These entities include the Vulkan 
    //       device and host, but may also include entities external to the Vulkan device or otherwise not part of the core Vulkan pipeline.When included in a source access mask, all writes that are performed by entities known to the Vulkan device are made available.When included in a destination access mask, makes all available writes visible to all future write accesses on entities known to the Vulkan device.
    //     - VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT specifies read access to a predicate as part of conditional rendering.
    //     - VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT specifies write access to a transform feedback buffer made 
    //       when transform feedback is active.
    //     - VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT specifies read access to a transform feedback counter buffer 
    //       which is read when vkCmdBeginTransformFeedbackEXT executes.
    //     - VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT specifies write access to a transform feedback counter buffer 
    //       which is written when vkCmdEndTransformFeedbackEXT executes.
    //     - VK_ACCESS_COMMAND_PROCESS_READ_BIT_NVX specifies reads from VkBuffer inputs to vkCmdProcessCommandsNVX.
    //     - VK_ACCESS_COMMAND_PROCESS_WRITE_BIT_NVX specifies writes to the target command buffer in vkCmdProcessCommandsNVX.
    //     - VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT is similar to VK_ACCESS_COLOR_ATTACHMENT_READ_BIT, but also 
    //       includes advanced blend operations.
    //     - VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV specifies read access to a shading rate image as part of a drawing command, 
    //       as bound by vkCmdBindShadingRateImageNV.
    //     - VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV specifies read access to an acceleration structure as 
    //       part of a trace or build command.
    //     - VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV specifies write access to an acceleration structure as 
    //       part of a build command.
    //     - VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT specifies read access to a fragment density map attachment during 
    //       dynamic fragment density map operations
    //
    //   - dependencyFlags is a bitmask of VkDependencyFlagBits.
    RenderPass(const LogicalDevice& logicalDevice,
               const std::vector<VkAttachmentDescription>& attachmentDescriptions,
               const std::vector<VkSubpassDescription>& subpassDescriptions,
               const std::vector<VkSubpassDependency>& subpassDependencies);
    ~RenderPass();
    RenderPass(RenderPass&& other) noexcept;
    RenderPass(const RenderPass&) = delete;
    const RenderPass& operator=(const RenderPass&) = delete;

    VkRenderPass 
    vkRenderPass() const;
    
private:
    const LogicalDevice& mLogicalDevice;
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
};
}

#endif
