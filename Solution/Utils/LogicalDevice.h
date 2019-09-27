#ifndef UTILS_LOGICAL_DEVICE
#define UTILS_LOGICAL_DEVICE

#include <vector>
#include <vulkan/vulkan.h>

#include "PhysicalDevice.h"

namespace vk {
class AppInstance;
class Surface;

// VkDevice wrapper.
// VkDevice objects represent logical connections to 
// physical devices. 
// The created logical device is then the primary interface 
// to the physical device.
// Each device exposes a number of queue families each having 
// one or more queues. 
// All queues in a queue family support the same operations.
//
// Vulkan separates the concept of physical and logical devices.
// A physical device usually represents a single complete implementation 
// of Vulkan(excluding instance - level functionality) available to the host, 
// of which there are a finite number.
// A logical device represents an instance of that implementation with its 
// own state and resources independent of other logical devices.
class LogicalDevice {
public:
    LogicalDevice(const AppInstance& appInstance, 
                  const Surface& surface);
    ~LogicalDevice();
    LogicalDevice(LogicalDevice&& other) noexcept;
    LogicalDevice(const LogicalDevice&) = delete;
    const LogicalDevice& operator=(const LogicalDevice&) = delete;

    VkDevice vkDevice() const;
    const PhysicalDevice& physicalDevice() const;

    VkQueue graphicsQueue() const;
    VkQueue transferQueue() const;
    VkQueue presentationQueue() const;   

private:
    void createLogicalDevice(const PhysicalDevice& physicalDevice);

    static std::vector<VkDeviceQueueCreateInfo> 
    buildDeviceQueueCreateInfoVector(const PhysicalDevice& physicalDevice);
   
    void setQueues();

    VkDevice mLogicalDevice = VK_NULL_HANDLE;

    // Command queues for graphics commands and presentation commands respectively.
    VkQueue mGraphicsQueue = VK_NULL_HANDLE;
    VkQueue mTransferQueue = VK_NULL_HANDLE;
    VkQueue mPresentationQueue = VK_NULL_HANDLE;

    PhysicalDevice* mPhysicalDevice = nullptr;
};
}

#endif