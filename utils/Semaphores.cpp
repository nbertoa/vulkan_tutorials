#include "Semaphores.h"

#include <cassert>

#include "LogicalDevice.h"

namespace vk {
Semaphores::Semaphores(const LogicalDevice& logicalDevice,
                       const size_t semaphoreCount) {
    assert(semaphoreCount > 0);

    mSemaphores.reserve(semaphoreCount);
    for (size_t i = 0; i < semaphoreCount; ++i) {
        Semaphore semaphore(logicalDevice);
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
    
    mCurrentSemaphore = (mCurrentSemaphore + 1) % mSemaphores.size();
    Semaphore& semaphore = mSemaphores[mCurrentSemaphore];

    return semaphore;
}

Semaphore&
Semaphores::currentSemaphore() {
    assert(mCurrentSemaphore < mSemaphores.size());
    return mSemaphores[mCurrentSemaphore];
}
}