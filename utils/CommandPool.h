#ifndef UTILS_COMMAND_POOL 
#define UTILS_COMMAND_POOL

#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkCommandPool wrapper.
// Command pools are opaque objects that command buffer 
// memory is allocated from, and which allow the implementation 
// to amortize the cost of resource creation across multiple 
// command buffers.
// Command pools are application-synchronized, meaning that 
// a command pool must not be used concurrently in multiple 
// threads.
// That includes use via recording command buffers allocated 
// from the pool, as well as operations that allocate, free, 
// and reset command buffers or the pool itself.
class CommandPool {
public:
    enum class Type {
        GRAPHICS,
        TRANSFER,
        PRESENTATION
    };

    // Flags:
    // - VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers
    // are rerecorded with new commands very often (may change memory
    // allocation behavior)
    //
    // - VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow 
    // command buffers to be rerecorded individually, without this flag
    // they all have to be reset together.
    CommandPool(const LogicalDevice& logicalDevice,
                const Type type,
                const VkCommandPoolCreateFlags flags = 0);
    ~CommandPool();
    CommandPool(CommandPool&& other) noexcept;

    CommandPool(const CommandPool&) = delete;
    const CommandPool& operator=(const CommandPool&) = delete;

    VkCommandPool vkCommandPool() const;

private:
    const LogicalDevice& mLogicalDevice;
    VkCommandPool mCommandPool = VK_NULL_HANDLE;
};
}

#endif