#include "Fences.h"

#include <cassert>

#include "LogicalDevice.h"

namespace vk {
Fences::Fences(const LogicalDevice& logicalDevice,
               const size_t fenceCount,
               const VkFenceCreateFlags flags) {
    assert(fenceCount > 0);

    mFences.reserve(fenceCount);
    for (size_t i = 0; i < fenceCount; ++i) {
        Fence fence(logicalDevice,
                    flags);
        mFences.emplace_back(std::move(fence));
    }
}

Fences::Fences(Fences&& other) noexcept
    : mFences(std::move(mFences))
    , mNextAvailableFence(other.mNextAvailableFence)
{

}

Fence&
Fences::nextAvailableFence() {
    assert(mFences.empty() == false);
    assert(mNextAvailableFence < mFences.size());

    Fence& fence = mFences[mNextAvailableFence];
    mNextAvailableFence = (mNextAvailableFence + 1) % mFences.size();

    return fence;
}
}