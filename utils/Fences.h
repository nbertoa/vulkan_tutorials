#ifndef UTILS_FENCES 
#define UTILS_FENCES

#include <vector>

#include "Fence.h"

namespace vk {
class Fences {
public:
    Fences(const LogicalDevice& logicalDevice,
           const size_t fenceCount);
    Fences(Fences&& other) noexcept;
               
    Fence& nextAvailableFence();
private:
    std::vector<Fence> mFences;
    size_t mNextAvailableFence = 0;
};
}

#endif 