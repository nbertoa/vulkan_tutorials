#ifndef UTILS_FENCE
#define UTILS_FENCE

#include <cassert>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkFence wrapper to be able to create and 
// handle them easily.
//  
// Fences are a synchronization primitive that
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
    Fence(const LogicalDevice& logicalDevice);
    ~Fence();
    Fence(Fence&& other) noexcept;

    Fence(const Fence&) = delete;
    const Fence& operator=(const Fence&) = delete;

    const VkFence& vkFence() const {
        assert(mFence != VK_NULL_HANDLE);
        return mFence;
    }

    void wait() const;
    void reset() const;
    void waitAndReset() const;

private:
    const LogicalDevice& mLogicalDevice;
    VkFence mFence = VK_NULL_HANDLE;
};
}

#endif 