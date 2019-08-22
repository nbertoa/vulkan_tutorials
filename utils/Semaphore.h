#ifndef UTILS_SEMAPHORE 
#define UTILS_SEMAPHORE

#include <cassert>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkSemaphore wrapper to be able to create and 
// handle them easily.
// Semaphores are a synchronization primitive that 
// can be used to insert a dependency between 
// batches submitted to queues.
//
// Semaphores are used to coordinate queue operations 
// both withing  a queue and between different queues.
//
// Semaphores have two states: signaled and unsignaled.
// The state of a semaphore can be signaled after 
// execution of a batch of commands is completed.
// A batch can wait for a semaphore to become 
// signaled before it begins execution, and the 
// semaphore is also unsignaled before the batch 
// begins execution.
class Semaphore {
public:
    Semaphore(const LogicalDevice& logicalDevice);
    ~Semaphore();
    
    const VkSemaphore& vkSemaphore() const {
        assert(mSemaphore != VK_NULL_HANDLE);
        return mSemaphore;
    }

private:
    const LogicalDevice& mLogicalDevice;
    VkSemaphore mSemaphore = VK_NULL_HANDLE;
};
}

#endif 