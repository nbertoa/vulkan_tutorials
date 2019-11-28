#ifndef UTILS_SYNC_SEMAPHORES 
#define UTILS_SYNC_SEMAPHORES

#include <limits>
#include <vector>

#include "Semaphore.h"

namespace vk {
class Semaphores {
public:
    Semaphores(const uint32_t semaphoreCount);
    Semaphores(Semaphores&& other) noexcept;
               
    Semaphore& 
    nextAvailableSemaphore();

    Semaphore& 
    currentSemaphore();
private:
    std::vector<Semaphore> mSemaphores;
    uint32_t mCurrentSemaphore = std::numeric_limits<uint32_t>::max();
};
}

#endif 