#ifndef UTILS_COMMAND_POOL 
#define UTILS_COMMAND_POOL

#include <cassert>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkCommandPool wrapper to be able to create/destroy/handle 
// it easily.
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
        PRESENTATION
    };

    CommandPool(const LogicalDevice& logicalDevice,
                const Type type);
    ~CommandPool();

    CommandPool(const CommandPool&) = delete;
    const CommandPool& operator=(const CommandPool&) = delete;

    VkCommandPool vkCommandPool() const { 
        assert(mCommandPool != VK_NULL_HANDLE);
        return mCommandPool;
    }

private:
    const LogicalDevice& mLogicalDevice;
    VkCommandPool mCommandPool = VK_NULL_HANDLE;
};
}

#endif