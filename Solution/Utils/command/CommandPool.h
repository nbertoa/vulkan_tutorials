#ifndef UTILS_COMMAND_COMMAND_POOL 
#define UTILS_COMMAND_COMMAND_POOL

#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;
class PhysicalDevice;

//
// VkCommandPool wrapper.
//
// CommandPool is a simple object that is used to allocate CommandBuffers.
// It is connected to a specific Queue Family.
//
// Command pools are opaque objects that command buffer 
// memory is allocated from, and which allow the implementation 
// to amortize the cost of resource creation across multiple 
// command buffers.
//
// Command pools are application-synchronized, meaning that 
// a command pool must not be used concurrently in multiple 
// threads.
// That includes use via recording command buffers allocated 
// from the pool, as well as operations that allocate, free, 
// and reset command buffers or the pool itself.
//
// You need the CommandPool to:
// - Create CommandBuffers
//
class CommandPool {
public:

    // * queueFamilyIndex designates a queue family. All command buffers allocated 
    //   from this command pool must be submitted on queues from the same queue family.
    //
    // * flags bitmask indicates usage behavior for the pool and 
    //   command buffers allocated from it (VK_COMMAND_POOL_CREATE_):
    // 
    //   - TRANSIENT_BIT, RESET_COMMAND_BUFFER_BIT, PROTECTED_BIT
    CommandPool(const LogicalDevice& logicalDevice,
                const uint32_t queueFamilyIndex,
                const VkCommandPoolCreateFlags flags = 0);
    ~CommandPool();
    CommandPool(CommandPool&& other) noexcept;
    CommandPool(const CommandPool&) = delete;
    const CommandPool& operator=(const CommandPool&) = delete;

    VkCommandPool 
    vkCommandPool() const;

private:
    const LogicalDevice& mLogicalDevice;
    VkCommandPool mCommandPool = VK_NULL_HANDLE;
};
}

#endif