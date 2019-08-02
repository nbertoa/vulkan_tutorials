#ifndef UTILS_LOGICAL_DEVICE
#define UTILS_LOGICAL_DEVICE

#include <cassert>
#include <vulkan/vulkan.h>

namespace vk {
class PhysicalDevice;

// VkDevice wrapper to be able to create/destroy/get it easily.
// VkDevice objects represent logical connections to physical devices. 
// The created logical device is then the primary interface to the physical device.
// Each device exposes a number of queue families each having one or more queues. 
// All queues in a queue family support the same operations.
class LogicalDevice {
public:
	LogicalDevice(const PhysicalDevice& physicalDevice);
	~LogicalDevice();

	VkQueue graphicsQueue() const { assert(mGraphicsQueue != VK_NULL_HANDLE); return mGraphicsQueue; }

private:
	VkDevice mDevice = VK_NULL_HANDLE;
	VkQueue mGraphicsQueue = VK_NULL_HANDLE;
};
}

#endif