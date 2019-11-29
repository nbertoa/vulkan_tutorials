#include "Semaphores.h"

#include <cassert>

#include "../device/LogicalDevice.h"

namespace vk2 {
Semaphores::Semaphores(const uint32_t semaphoreCount) {
    assert(semaphoreCount > 0);

    mSemaphores.reserve(semaphoreCount);
    for (uint32_t i = 0; i < semaphoreCount; ++i) {
        Semaphore semaphore;
        mSemaphores.emplace_back(std::move(semaphore));
    }
}

Semaphores::Semaphores(Semaphores&& other) noexcept
    : mSemaphores(std::move(mSemaphores))
    , mCurrentSemaphore(other.mCurrentSemaphore)
{

}

Semaphore& 
Semaphores::nextAvailableSemaphore() {
    assert(mSemaphores.empty() == false);
    
    mCurrentSemaphore = (mCurrentSemaphore + 1) % static_cast<uint32_t>(mSemaphores.size());
    Semaphore& semaphore = mSemaphores[mCurrentSemaphore];

    return semaphore;
}

Semaphore&
Semaphores::currentSemaphore() {
    assert(mCurrentSemaphore < mSemaphores.size());
    return mSemaphores[mCurrentSemaphore];
}
}