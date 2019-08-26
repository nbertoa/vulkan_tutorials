#ifndef UTILS_SEMAPHORES 
#define UTILS_SEMAPHORES

#include <vector>

#include "Semaphore.h"

namespace vk {
class Semaphores {
public:
    Semaphores(const LogicalDevice& logicalDevice,
               const size_t semaphoreCount);
    Semaphores(Semaphores&& other) noexcept;
               
    Semaphore& nextAvailableSemaphore();
private:
    std::vector<Semaphore> mSemaphores;
    size_t mNextAvailableSemaphore = 0;
};
}

#endif 