#include "Fences.h"

#include <cassert>

#include "../device/LogicalDevice.h"

namespace vk2 {
Fences::Fences(const size_t fenceCount,
               const vk::FenceCreateFlagBits flags) {
    assert(fenceCount > 0);

    vk::Device device(LogicalDevice::device());

    mFences.reserve(fenceCount);
    for (size_t i = 0; i < fenceCount; ++i) {
        mFences.emplace_back(device.createFenceUnique({flags}));
    }
}

Fences::Fences(Fences&& other) noexcept
    : mFences(std::move(mFences))
    , mCurrentFence(other.mCurrentFence)
{

}

vk::Fence&
Fences::nextAvailableFence() {
    assert(mFences.empty() == false);

    mCurrentFence = (mCurrentFence + 1) % static_cast<uint32_t>(mFences.size());
    vk::UniqueFence& fence = mFences[mCurrentFence];    

    return fence.get();
}

vk::Fence& 
Fences::currentFence() {
    assert(mCurrentFence < mFences.size());
    return mFences[mCurrentFence].get();
}
}