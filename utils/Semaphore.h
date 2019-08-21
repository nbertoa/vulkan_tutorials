#ifndef UTILS_SEMAPHORE 
#define UTILS_SEMAPHORE

#include <cassert>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

class Semaphore {
public:
    Semaphore(const LogicalDevice& logicalDevice);
    ~Semaphore();

    VkSemaphore vkSemaphore() const {
        assert(mSemaphore != VK_NULL_HANDLE);
        return mSemaphore;
    }

private:
    const LogicalDevice& mLogicalDevice;
    VkSemaphore mSemaphore = VK_NULL_HANDLE;
};
}

#endif 