#ifndef UTILS_FENCES 
#define UTILS_FENCES

#include <limits>
#include <vector>

#include "Fence.h"

namespace vk {
class Fences {
public:
    // - flags specifies the initial state and behavior of the fences.
    // VkFenceCreateFlags:
    // VK_FENCE_CREATE_SIGNALED_BIT specifies that the fence object 
    // is created in the signaled state.
    // Otherwise, it is created in the unsignaled state.
    Fences(const LogicalDevice& logicalDevice,
           const size_t fenceCount,
           const VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
    Fences(Fences&& other) noexcept;
               
    Fence& 
    nextAvailableFence();

    Fence& 
    currentFence();

private:
    std::vector<Fence> mFences;
    size_t mCurrentFence = std::numeric_limits<size_t>::max();
};
}

#endif 