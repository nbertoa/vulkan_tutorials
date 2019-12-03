#include "Semaphores.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk2 {
Semaphores::Semaphores(const size_t semaphoreCount) {
    assert(semaphoreCount > 0);
    
    vk::Device device(LogicalDevice::device());
    mSemaphores.reserve(semaphoreCount);
    for (size_t i = 0; i < semaphoreCount; ++i) {
        mSemaphores.emplace_back(device.createSemaphoreUnique({}));
    }
}

Semaphores::Semaphores(Semaphores&& other) noexcept
    : mSemaphores(std::move(mSemaphores))
    , mCurrentSemaphore(other.mCurrentSemaphore)
{

}

vk::Semaphore& 
Semaphores::nextAvailableSemaphore() {
    assert(mSemaphores.empty() == false);
    
    mCurrentSemaphore = (mCurrentSemaphore + 1) % static_cast<uint32_t>(mSemaphores.size());
    vk::UniqueSemaphore& semaphore = mSemaphores[mCurrentSemaphore];

    return semaphore.get();
}

vk::Semaphore&
Semaphores::currentSemaphore() {
    assert(mCurrentSemaphore < mSemaphores.size());
    return mSemaphores[mCurrentSemaphore].get();
}
}