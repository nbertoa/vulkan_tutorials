#ifndef UTILS_SYNC_SEMAPHORES 
#define UTILS_SYNC_SEMAPHORES

#include <limits>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vulkan {
class Semaphores {
public:
    Semaphores(const size_t semaphoreCount);
    Semaphores(Semaphores&& other) noexcept;
               
    vk::Semaphore 
    nextAvailableSemaphore();

    vk::Semaphore 
    currentSemaphore();
private:
    std::vector<vk::UniqueSemaphore> mSemaphores;
    uint32_t mCurrentSemaphore = std::numeric_limits<uint32_t>::max();
};
}

#endif 