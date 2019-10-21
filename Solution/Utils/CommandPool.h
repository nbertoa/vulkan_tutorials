#ifndef UTILS_COMMAND_POOL 
#define UTILS_COMMAND_POOL

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
    // * flags bitmask indicates usage behavior for the pool and command buffers allocated from it:
    // 
    //   - VK_COMMAND_POOL_CREATE_TRANSIENT_BIT specifies that command buffers allocated 
    //     from the pool will be short-lived, meaning that they will be reset or freed 
    //     in a relatively short timeframe. This flag may be used by the implementation to 
    //     control memory allocation behavior within the pool.
    //   - VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT allows any command buffer 
    //     allocated from a pool to be individually reset to the initial state; either by 
    //     calling vkResetCommandBuffer, or via the implicit reset when calling vkBeginCommandBuffer.
    //     If this flag is not set on a pool, then vkResetCommandBuffer must not be called for 
    //     any command buffer allocated from that pool.
    //   - VK_COMMAND_POOL_CREATE_PROTECTED_BIT specifies that command buffers allocated 
    //     from the pool are protected command buffers. If the protected memory feature is not enabled, 
    //     the VK_COMMAND_POOL_CREATE_PROTECTED_BIT bit of flags must not be set.
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