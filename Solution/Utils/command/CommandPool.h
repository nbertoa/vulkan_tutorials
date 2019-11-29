#ifndef UTILS_COMMAND_COMMAND_POOL 
#define UTILS_COMMAND_COMMAND_POOL

#include <vulkan/vulkan.h>

#include "CommandBuffer.h"

namespace vk2 {
class PhysicalDevice;

//
// VkCommandPool wrapper.
//
// Command pools are opaque objects that command buffer memory is allocated from, 
// and which allow the implementation to amortize the cost of 
// resource creation across multiple command buffers.
//
// Command pools are externally synchronized, meaning that a 
// command pool must not be used concurrently in multiple threads.
//
// That includes use via recording commands on any command buffers allocated from the pool, 
// as well as operations that allocate, free, and reset command buffers or the pool itself.
//
// You need the CommandPool to:
// - Allocate CommandBuffers through vkAllocateCommandBuffer
//
class CommandPool {
public:
    // * queueFamilyIndex where all command buffers, allocated 
    //   from this command pool, must be submitted on.
    //
    // * flags bitmask indicates usage behavior for the pool and 
    //   command buffers allocated from it (VK_COMMAND_POOL_CREATE_):
    // 
    //   - TRANSIENT_BIT, RESET_COMMAND_BUFFER_BIT, PROTECTED_BIT
    //
    // Notes: The global logical device is the device that creates the command pool.
    CommandPool(const uint32_t queueFamilyIndex,
                const VkCommandPoolCreateFlags flags = 0);
    ~CommandPool();
    CommandPool(CommandPool&& other) noexcept;
    CommandPool(const CommandPool&) = delete;
    const CommandPool& operator=(const CommandPool&) = delete;

    VkCommandPool
    vkCommandPool() const;

    // Returns a command buffer that already begun the recording
    // with flag VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT.
    CommandBuffer
    createAndBeginOneTimeSubmitCommandBuffer() const;

private:
  VkCommandPool mCommandPool = VK_NULL_HANDLE;
};
}

#endif