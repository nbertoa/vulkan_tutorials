#ifndef UTILS_FENCE
#define UTILS_FENCE

#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkFence wrapper.
//  
// Fences are a synchronization primitiveS that
// can be used to insert a dependency from a 
// queue to the host.
//
// Fences can be used by the host to determine 
// completion of execution of queue operations.
//
// Fences have two states: signaled and unsignaled.
// A fence can be signaled as part of the execution
// of a queue submission command.
// The host can poll the status of a single fence,
// or wait for any or all of a group of fences
// to become signaled.
class Fence {
public:
    // Flags:
    // VK_FENCE_CREATE_SIGNALED_BIT specifies that the fence object 
    // is created in the signaled state.
    // Otherwise, it is created in the unsignaled state.
    Fence(const LogicalDevice& logicalDevice,
          const VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
    ~Fence();
    Fence(Fence&& other) noexcept;
    Fence(const Fence&) = delete;
    const Fence& operator=(const Fence&) = delete;

    const VkFence& vkFence() const;

    void wait() const;
    void reset() const;
    void waitAndReset() const;

private:
    const LogicalDevice& mLogicalDevice;
    VkFence mFence = VK_NULL_HANDLE;
};
}

#endif 