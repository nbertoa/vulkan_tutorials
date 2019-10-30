#include "Fences.h"

#include <cassert>

#include "LogicalDevice.h"

namespace vk {
Fences::Fences(const LogicalDevice& logicalDevice,
               const uint32_t fenceCount,
               const VkFenceCreateFlags flags) {
    assert(fenceCount > 0);

    mFences.reserve(fenceCount);
    for (uint32_t i = 0; i < fenceCount; ++i) {
        Fence fence(logicalDevice,
                    flags);
        mFences.emplace_back(std::move(fence));
    }
}

Fences::Fences(Fences&& other) noexcept
    : mFences(std::move(mFences))
    , mCurrentFence(other.mCurrentFence)
{

}

Fence&
Fences::nextAvailableFence() {
    assert(mFences.empty() == false);

    mCurrentFence = (mCurrentFence + 1) % mFences.size();
    Fence& fence = mFences[mCurrentFence];    

    return fence;
}

Fence& 
Fences::currentFence() {
    assert(mCurrentFence < mFences.size());
    return mFences[mCurrentFence];
}
}