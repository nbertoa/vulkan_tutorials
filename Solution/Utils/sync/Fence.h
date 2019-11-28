#ifndef UTILS_SYNC_FENCE
#define UTILS_SYNC_FENCE

#include <vulkan/vulkan.h>

namespace vk {
//
// VkFence wrapper.
//
// Fences are a synchronization primitive that can be used to 
// insert a dependency from a queue to the host.
// It can be waited on, polled, and manually unsignaled on the host.
//
// Fences have two states - signaled and unsignaled.
// - A fence can be signaled as part of the execution of a 
//   queue submission command. A fence signals to the host 
//   that the execution of a task has completed.
// - Fences can be unsignaled on the host with vkResetFences.
//
// Fences can be waited on by the host with the vkWaitForFences command, 
// and the current state can be queried with vkGetFenceStatus.
//
// As with most objects in Vulkan, fences are an interface to internal data which 
// is typically opaque to applications. This internal data is referred to as a fence’s payload.
// However, in order to enable communication with agents outside 
// of the current device, it is necessary to be able to export that payload 
// to a commonly understood format, and subsequently import from that format as well.
//
// The internal data of a fence may include a reference to any resources and pending 
// work associated with signal or unsignal operations performed on that fence object.
//
// You need the Fence to:
// - Pass it to vkQueueSubmit
//
class Fence {
public:
    // * flags specifies the initial state and behavior of the fence:
    // 
    //   - VK_FENCE_CREATE_SIGNALED_BIT specifies that the fence object 
    //     is created in the signaled state.
    //   - Otherwise, it is created in the unsignaled state.
    //
    // Notes: The global logical devices creates the fence
    Fence(const VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
    ~Fence();
    Fence(Fence&& other) noexcept;
    Fence(const Fence&) = delete;
    const Fence& operator=(const Fence&) = delete;

    const VkFence& 
    vkFence() const;

    void 
    wait() const;

    void 
    reset() const;

    void 
    waitAndReset() const;

private:
    VkFence mFence = VK_NULL_HANDLE;
};
}

#endif 