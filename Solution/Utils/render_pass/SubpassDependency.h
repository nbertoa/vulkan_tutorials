#ifndef UTILS_RENDER_PASS_SUBPASS_DEPENDENCY
#define UTILS_RENDER_PASS_SUBPASS_DEPENDENCY

#include <vulkan/vulkan.h>

namespace vk2 {
//
// VkSubpassDependency wrapper
//
// We need to tell to the render pass the dependencies between pairs of subpasses.
//
// You need the SubpassDependency to:
// - Create the RenderPass.
//
class SubpassDependency {
public:
    // * sourceSubpassIndex of the first subpass in the dependency, or VK_SUBPASS_EXTERNAL.
    //
    // * destSubpassIndex of the second subpass in the dependency, or VK_SUBPASS_EXTERNAL.
    //
    // * sourceStageMask bitmask specifying a set of source pipeline stages:
    // * destStageMask bitmask specifying a set of destination pipeline stages (VK_PIPELINE_STAGE_):
    //
    //   - TOP_OF_PIPE_BIT, DRAW_INDIRECT_BIT, TASK_SHADER_BIT_NV,
    //     MESH_SHADER_BIT_NV, VERTEX_INPUT_BIT, VERTEX_SHADER_BIT,
    //     TESSELLATION_CONTROL_SHADER_BIT, TESSELLATION_EVALUATION_SHADER_BIT,
    //     GEOMETRY_SHADER_BIT, FRAGMENT_SHADER_BIT, EARLY_FRAGMENT_TESTS_BIT, 
    //     LATE_FRAGMENT_TESTS_BIT, COLOR_ATTACHMENT_OUTPUT_BIT, COMPUTE_SHADER_BIT,
    //     TRANSFER_BIT, BOTTOM_OF_PIPE_BIT, HOST_BIT, RAY_TRACING_SHADER_BIT_NV,
    //     ALL_GRAPHICS_BIT, ALL_COMMANDS_BIT, CONDITIONAL_RENDERING_BIT_EXT,
    //     TRANSFORM_FEEDBACK_BIT_EXT, COMMAND_PROCESS_BIT_NVX, SHADING_RATE_IMAGE_BIT_NV,
    //     FRAGMENT_DENSITY_PROCESS_BIT_EXT
    //
    // * sourceAccessMask bitmask specifying source access mask. 
    //   This access scope is limited to access in the 
    //   pipeline stages determined by the source stage mask specified by sourceStageMask. 
    //   It is also limited to access types in the source access mask specified by sourceAccessMask:
    // * destAccessMask bistmask specifying destination access mask. 
    //   This access scope is limited to access in the pipeline stages determined by the 
    //   destination stage mask specified by destStageMask. 
    //   It is also limited to access types in the destination access mask 
    //   specified by destAccessMask (VK_ACCESS_):
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
    //     ACCELERATION_STRUCTURE_WRITE_BIT_NV,
    //     FRAGMENT_DENSITY_MAP_READ_BIT_EXT
    //
    // * dependencyFlags bitmask. The availability and visibility operations defined by a 
    //   subpass dependency affect the execution of image layout transitions 
    //   within the render pass (VK_DEPENDENCY_):
    //
    //   - BY_REGION_BIT, VIEW_LOCAL_BIT, DEVICE_GROUP_BIT.
    //
    // Notes:
    // If sourceSubpassIndex is equal to destSubpassIndex then this dependency describes 
    // a subpass self-dependency, and only constrains the pipeline barriers allowed 
    // within a subpass instance. 
    // Otherwise, when a render pass instance which includes a subpass dependency 
    // is submitted to a queue, it defines a memory dependency between the subpasses 
    // identified by sourceSubpassIndex and destSubpassIndex.
    //
    // If sourceSubpassIndex is equal to VK_SUBPASS_EXTERNAL, the first synchronization scope 
    // includes commands that occur earlier in submission order than the vkCmdBeginRenderPass 
    // used to begin the render pass instance.
    // Otherwise, the first set of commands includes all commands submitted as part of the 
    // subpass instance identified by srcSubpass
    // and any load, store or multisample resolve operations on attachments used in srcSubpass.
    // In either case, the first synchronization scope is limited to operations on 
    // the pipeline stages determined by the source stage mask specified by srcStageMask.
    //
    // If destSubpassIndex is equal to VK_SUBPASS_EXTERNAL, the second synchronization scope 
    // includes commands that occur later 
    // in submission order than the vkCmdEndRenderPass used to end the render pass instance.
    // Otherwise, the second set of commands includes all commands submitted as part of the 
    // subpass instance identified by destSubpassIndex 
    // and any load, store or multisample resolve operations on attachments used in 
    // destSubpassIndex. 
    // In either case, the second synchronization scope is limited to operations on the 
    // pipeline stages determined by the destination stage mask specified by destStageMask.
    //
    // The sourceAccessMask scope is limited to access in the pipeline stages determined 
    // by the source stage mask specified by sourceStageMask. 
    // It is also limited to access types in the source access mask specified by sourceAccessMask.
    //
    // The destAccessMask scope is limited to access in the pipeline stages determined 
    // by the destination stage mask specified by destStageMask.
    // It is also limited to access types in the destination access mask specified by dstAccessMask.
    SubpassDependency(const uint32_t sourceSubpassIndex,
                      const uint32_t destSubpassIndex,
                      const VkPipelineStageFlags sourceStageMask,
                      const VkAccessFlags sourceAccessMask,
                      const VkPipelineStageFlags destStageMask,
                      const VkAccessFlags destAccessMask,
                      const VkDependencyFlags dependencyFlags = {});

private:
    VkSubpassDependency mSubpassDependency = {};
};
}

#endif 