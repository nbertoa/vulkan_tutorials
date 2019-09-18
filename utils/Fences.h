#ifndef UTILS_FENCES 
#define UTILS_FENCES

#include <vector>

#include "Fence.h"

namespace vk {
class Fences {
public:
    // Flags:
    // VK_FENCE_CREATE_SIGNALED_BIT specifies that the fence object 
    // is created in the signaled state.
    // Otherwise, it is created in the unsignaled state.
    Fences(const LogicalDevice& logicalDevice,
           const size_t fenceCount,
           const VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
    Fences(Fences&& other) noexcept;
               
    Fence& nextAvailableFence();
private:
    std::vector<Fence> mFences;
    size_t mNextAvailableFence = 0;
};
}

#endif 