#ifndef UTILS_SYNC_FENCES 
#define UTILS_SYNC_FENCES

#include <limits>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vk2 {
class Fences {
public:
    // Read Fence to understand this
    Fences(const uint32_t fenceCount,
           const vk::FenceCreateFlagBits flags = vk::FenceCreateFlagBits::eSignaled);
    Fences(Fences&& other) noexcept;
               
    vk::Fence& 
    nextAvailableFence();

    vk::Fence& 
    currentFence();

private:
    std::vector<vk::UniqueFence> mFences;
    uint32_t mCurrentFence = std::numeric_limits<uint32_t>::max();
};
}

#endif 