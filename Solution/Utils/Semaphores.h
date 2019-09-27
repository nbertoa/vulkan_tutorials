#ifndef UTILS_SEMAPHORES 
#define UTILS_SEMAPHORES

#include <limits>
#include <vector>

#include "Semaphore.h"

namespace vk {
class Semaphores {
public:
    Semaphores(const LogicalDevice& logicalDevice,
               const size_t semaphoreCount);
    Semaphores(Semaphores&& other) noexcept;
               
    Semaphore& nextAvailableSemaphore();
    Semaphore& currentSemaphore();
private:
    std::vector<Semaphore> mSemaphores;
    size_t mCurrentSemaphore = std::numeric_limits<size_t>::max();
};
}

#endif 