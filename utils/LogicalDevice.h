#ifndef UTILS_LOGICAL_DEVICE
#define UTILS_LOGICAL_DEVICE

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

#include "PhysicalDevice.h"

namespace vk {
class AppInstance;
class Surface;

// VkDevice wrapper to be able to create/destroy/get it easily.
// VkDevice objects represent logical connections to 
// physical devices. 
// The created logical device is then the primary interface 
// to the physical device.
// Each device exposes a number of queue families each having 
// one or more queues. 
// All queues in a queue family support the same operations.
class LogicalDevice {
public:
    LogicalDevice(const AppInstance& appInstance, 
                  const Surface& surface);
    ~LogicalDevice();
    LogicalDevice(LogicalDevice&& other) noexcept;

    LogicalDevice(const LogicalDevice&) = delete;
    const LogicalDevice& operator=(const LogicalDevice&) = delete;

    VkDevice vkDevice() const { 
        assert(mLogicalDevice != VK_NULL_HANDLE);  
        return mLogicalDevice; 
    }

    VkQueue graphicsQueue() const { 
        assert(mGraphicsQueue != VK_NULL_HANDLE); 
        return mGraphicsQueue; 
    }

    VkQueue presentationQueue() const { 
        assert(mPresentationQueue != VK_NULL_HANDLE); 
        return mPresentationQueue; 
    }

    const PhysicalDevice& physicalDevice() const { 
        assert(mPhysicalDevice != nullptr); 
        return *mPhysicalDevice; 
    }

private:
    void createLogicalDevice(const PhysicalDevice& physicalDevice);

    static std::vector<VkDeviceQueueCreateInfo> 
    buildDeviceQueueCreateInfoVector(const PhysicalDevice& physicalDevice);
   
    void setQueues();

    VkDevice mLogicalDevice = VK_NULL_HANDLE;

    // Command queues for graphics commands and presentation commands respectively.
    VkQueue mGraphicsQueue = VK_NULL_HANDLE;
    VkQueue mPresentationQueue = VK_NULL_HANDLE;

    PhysicalDevice* mPhysicalDevice = nullptr;
};
}

#endif