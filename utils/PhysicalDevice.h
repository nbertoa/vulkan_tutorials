#ifndef UTILS_PHYSICAL_DEVICE
#define UTILS_PHYSICAL_DEVICE

#include <vulkan/vulkan.h>

namespace vk {
class DebugMessenger;

// VkPhysicalDevice wrapper to be able to create/destroy/get it easiy.
class PhysicalDevice {
public:
	PhysicalDevice(const VkInstance& instance);

private:
	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
};
}

#endif