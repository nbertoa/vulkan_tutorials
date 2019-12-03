#ifndef UTILS_COMMAND_COMMAND_BUFFER
#define UTILS_COMMAND_COMMAND_BUFFER

#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.hpp>

namespace vk2 {
class Buffer;
class CommandPool;
class Fence;
class GraphicsPipeline;
class Image;
class ImageMemoryBarrier;
class PipelineLayout;
class RenderPass;

//
// VkCommandBuffer wrapper.
//
// Command buffers are objects used to record commands 
// which can be subsequently submitted to a device queue for execution.  
//
// Command buffers are objects used to specify the order, type and parameters of tasks 
// that should be performed when the CommandBuffer is submitted to a queue 
// and is finally consumed by the LogicalDevice.
// It represents a buffer of various commands to be executed by a LogicalDevice.
//
// There are two levels of command buffers: 
// - Primary command buffers which can execute secondary 
//   command buffers, and which are submitted to queues.
// - Secondary command buffers, which can be executed by 
//   primary command buffers, and which are not directly
//   submitted to queues.
//
// Each command buffer is always in one of the following states:
// - Initial: When a command buffer is allocated, it is in the initial state.
//   Some commands are able to reset a command buffer, or a set of command buffers, 
//   back to this state from any of the executable, recording or invalid state.
//   Command buffers in the initial state can only be moved to the recording state, or freed.
//
// - Recording: vkBeginCommandBuffer changes the state of a command buffer 
//   from the initial state to the recording state. 
//   Once a command buffer is in the recording state, 
//   vkCmd* commands can be used to record to the command buffer.
//
// - Executable: vkEndCommandBuffer ends the recording of a command buffer, 
//   moves it from the recording state to the executable state.
//   Executable command buffers can be submitted, reset, or recorded to another command buffer.
//
// - Pending: Queue submission of a command buffer changes the state of a command buffer 
//   from the executable state to the pending state. Whilst in the pending state, 
//   applications must not attempt to modify the command buffer in any way-as the device may 
//   be processing the commands recorded to it.
//   Once execution of a command buffer completes, the command buffer reverts back 
//   to either the executable state, or the invalid state if it was recorded 
//   with VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT.
//   A synchronization command should be used to detect when this occurs.
//
// - Invalid: Some operations, such as modifying or deleting a resource that was used 
//   in a command recorded to a command buffer, will transition the state of that command buffer 
//   into the invalid state.Command buffers in the invalid state can only be reset or freed.
//
// You need the CommandBuffer to:
// - Submit it to the Queue using vkQueueSubmit
//
// To create/use the CommandBuffer you need:
// - CommandPool
// - PipelineLayout and DescriptorSet for vkCmdBindDescriptorSets
// - FrameBuffer and RenderPass for vkCmdBeginRenderPass
// - GraphicsPipeline for vkCmdBindPipeline
//
class CommandBuffer {
public:
    // * commandPool from which the command buffer is allocated.
    //   The global logical device owns the command pool
    //
    // * level for command buffer (VK_COMMAND_BUFFER_):
    // 
    //   - LEVEL_PRIMARY, LEVEL_SECONDARY
    CommandBuffer(const CommandPool& commandPool,
                  const VkCommandBufferLevel level);
    CommandBuffer(const VkCommandBuffer commandBuffer);
    CommandBuffer(CommandBuffer&& other) noexcept;
    CommandBuffer(const CommandBuffer&) = delete;
    const CommandBuffer& operator=(const CommandBuffer&) = delete;    

    // * usageFlags bitmask specifying usage behavior 
    //   for the command buffer (VK_COMMAND_BUFFER_USAGE_):
    //
    //   - ONE_TIME_SUBMIT_BIT, SIMULTANEOUS_USE_BIT
    void 
    beginRecording(const VkCommandBufferUsageFlags usageFlags);
    
    void 
    endRecording();

    // * renderPass to begin an instance of.
    //
    // * framebuffer that contains the attachments 
    //   that are used with the render pass.
    //
    // * imageExtent of the render area 
    //   that is affected by the render pass.
    //
    // * clearValues contains clear values for each attachment, if the attachment 
    //   uses a loadOp value of VK_ATTACHMENT_LOAD_OP_CLEAR or 
    //   if the attachment has a depth/stencil format and uses a stencilLoadOp value 
    //   of VK_ATTACHMENT_LOAD_OP_CLEAR.
    //   The array is indexed by attachment number.
    //   Only elements corresponding to cleared attachments are used.
    //   Other elements of clearValues are ignored.
    void 
    beginPass(const vk::RenderPass renderPass,
              const vk::Framebuffer frameBuffer,
              const vk::Extent2D& imageExtent);
    
    void 
    endPass();

    void 
    bindPipeline(const GraphicsPipeline& graphicsPipeline);

    // * pipelineLayout used to program the bindings
    //
    // * descriptorSet to write to.
    //
    // Notes:
    // Once bound, a descriptor set affects rendering of subsequent graphics commands 
    // in the command buffer until a different set is bound to the same set number, 
    // or else until the set is disturbed.
    void
    bindDescriptorSet(const vk::PipelineLayout pipelineLayout,
                      const VkDescriptorSet descriptorSet);

    void 
    bindVertexBuffer(const Buffer& buffer);

    void 
    bindIndexBuffer(const Buffer& buffer,
                    const VkIndexType indexType);

    // * regionToCopy:
    //
    //   - srcOffset is the starting offset in bytes from the start of sourceBuffer.
    //   - dstOffset is the starting offset in bytes from the start of destinationBuffer.
    //   - size in bytes to copy.
    void 
    copyBuffer(const Buffer& sourceBuffer,
               const Buffer& destinationBuffer,
               const VkBufferCopy& regionToCopy);

    void
    copyBufferToImage(const Buffer& sourceBuffer,
                      const Image& destinationImage,
                      const VkBufferImageCopy& regionToCopy,
                      const VkImageLayout destImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // When vkCmdPipelineBarrier (by calling this method) is submitted to a queue, it defines 
    // a memory dependency between commands that were submitted before it, 
    // and those submitted after it.
    //
    // If vkCmdPipelineBarrier was recorded outside a render pass instance, 
    // the first synchronization scope includes all commands that occur 
    // earlier in submission order.
    //
    // If vkCmdPipelineBarrier was recorded inside a render pass instance, 
    // the first synchronization scope includes only commands that occur earlier 
    // in submission order within the same subpass.
    //
    // In either case, the first synchronization scope is limited to operations 
    // on the pipeline stages determined by the sourceStageMask.
    //
    // If vkCmdPipelineBarrier was recorded outside a render pass instance, 
    // the second synchronization scope includes all commands that occur later in submission order.
    //
    // If vkCmdPipelineBarrier was recorded inside a render pass instance, 
    // the second synchronization scope includes only commands that occur 
    // later in submission order within the same subpass.
    //
    // In either case, the second synchronization scope is limited to 
    // operations on the pipeline stages determined by the destStageMask.
    //
    // * sourceStageMask specifying a set of source pipeline stages.
    //   The first access scope is limited to access in the pipeline stages determined by
    //   the sourceStageMask.
    //   Within that, the first access scope only includes the first access scopes 
    //   defined by imageMemoryBarrier.
    // * destStageMask specifying a set of destination pipeline stages.
    //   The second access scope is limited to access in the pipeline stages 
    //   determined by the destStageMask.
    //   Within that, the second access scope only includes the second access scopes 
    //   defined by imageMemoryBarrier.
    //
    //   - The pipeline barrier specifies an execution dependency such that all work performed 
    //     by the set of pipeline stages included in sourceStageMask of the first set of 
    //     commands completes before any work performed by the set of pipeline stages 
    //     included in destStageMask of the second set of commands begins.
    //     Read submit() comments to check the possible values of these parameters.
    //
    // * dependencyFlags. If it contains VK_DEPENDENCY_BY_REGION_BIT, 
    //   then the dependency is by-region.
    void
    imagePipelineBarrier(const ImageMemoryBarrier& imageMemoryBarrier,
                         const VkPipelineStageFlags sourceStageMask,
                         const VkPipelineStageFlags destStageMask,
                         const VkDependencyFlags dependencyFlags = 0);

    void 
    draw(const uint32_t vertexCount,
         const uint32_t instanceCount = 1,
         const uint32_t firstVertex = 0,
         const uint32_t firstInstance = 0);

    void 
    drawIndexed(const uint32_t indexCount,
                const uint32_t instanceCount = 1,
                const uint32_t firstIndex = 0,
                const uint32_t vertexOffset = 0,
                const uint32_t firstInstance = 0);

    // * waitSemaphore to wait before this command buffer begin execution.
    //   If it is provided, then it defines a semaphore wait operation.
    //
    // * signalSemaphore that will be signaled when 
    //   this command buffer has completed execution.
    //   If it is provided, then it defines a semaphore signal operation.
    //
    // * executionCompletedFence to be signaled once this
    //   command buffer has completed execution. 
    //
    // * waitStageFlags is a pipeline stage at which 
    //   the waitSemaphore wait will occur (VK_PIPELINE_STAGE_):
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
    void 
    submit(const VkQueue queue,
           const vk::Semaphore* waitSemaphore,
           const vk::Semaphore* signalSemaphore,
           const vk::Fence& executionCompletedFence,
           const VkPipelineStageFlags waitStageFlags);

private:
    VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
};
}

#endif