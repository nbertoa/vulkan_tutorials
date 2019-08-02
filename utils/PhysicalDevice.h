#ifndef UTILS_PHYSICAL_DEVICE
#define UTILS_PHYSICAL_DEVICE

#include <cassert>
#include <vulkan/vulkan.h>

namespace vk {
// VkPhysicalDevice wrapper to be able to create/destroy/get it easily.
// It represents a physical device installed in the system.
class PhysicalDevice {
public:
	PhysicalDevice(const VkInstance instance, const VkSurfaceKHR surface);

	VkPhysicalDevice vkPhysicalDevice() const { assert(mDevice != VK_NULL_HANDLE); return mDevice; }
	uint32_t queueFamilyIndex() const { assert(mDevice != VK_NULL_HANDLE); return mQueueFamilyIndex; }

private:
	// Check if the device supports all the queue families that we need to use.
	static bool areQueueFamiliesSupported(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t& queueFamilyIndex);
	
	static bool isSurfaceSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, const uint32_t queueFamilyIndex);

	VkPhysicalDevice mDevice = VK_NULL_HANDLE;
	uint32_t mQueueFamilyIndex = 0;
};
}

#endif