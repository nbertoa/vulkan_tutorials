#ifndef UTILS_SYNC_FENCES 
#define UTILS_SYNC_FENCES

#include <limits>
#include <vector>

#include "Fence.h"

namespace vk {
class Fences {
public:
    // Read Fence to understand this
    Fences(const uint32_t fenceCount,
           const VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
    Fences(Fences&& other) noexcept;
               
    Fence& 
    nextAvailableFence();

    Fence& 
    currentFence();

private:
    std::vector<Fence> mFences;
    uint32_t mCurrentFence = std::numeric_limits<uint32_t>::max();
};
}

#endif 