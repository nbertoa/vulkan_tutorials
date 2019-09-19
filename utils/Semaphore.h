#ifndef UTILS_SEMAPHORE 
#define UTILS_SEMAPHORE

#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkSemaphore wrapper.
// Semaphores are a synchronization primitive that can be used to insert a 
// dependency between batches submitted to queues. 
// Semaphores have two states - signaled and unsignaled. 
// The state of a semaphore can be signaled after 
// execution of a batch of commands is completed. 
// A batch can wait for a semaphore to become signaled 
// before it begins execution, and the semaphore is 
// also unsignaled before the batch begins execution.
//
// As with most objects in Vulkan, semaphores are an 
// interface to internal data which is typically opaque 
// to applications.
// This internal data is referred to as a semaphore’s payload.
//
// However, in order to enable communication with agents 
// outside of the current device, it is necessary to be able 
// to export that payload to a commonly understood format, 
// and subsequently import from that format as well.
// 
// The internal data of a semaphore may include a reference 
// to any resourcesand pending work associated with signal or 
// unsignal operations performed on that semaphore object.
// Mechanisms to importand export that internal data toand from 
// semaphores are provided below.These mechanisms indirectly enable 
// applications to share semaphore state between two or more semaphores
// and other synchronization primitives across processand API boundaries.
class Semaphore {
public:
    Semaphore(const LogicalDevice& logicalDevice);    
    ~Semaphore();
    Semaphore(Semaphore&& other) noexcept;

    Semaphore(const Semaphore&) = delete;
    const Semaphore& operator=(const Semaphore&) = delete;
    
    const VkSemaphore& vkSemaphore() const;

private:
    const LogicalDevice& mLogicalDevice;
    VkSemaphore mSemaphore = VK_NULL_HANDLE;
};
}

#endif 