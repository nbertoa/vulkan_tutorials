#include "PhysicalDevice.h"

#include <vector>

#include "DebugUtils.h"

namespace vk {
PhysicalDevice::PhysicalDevice(const VkInstance& instance)
{
	assert(instance != VK_NULL_HANDLE);

	uint32_t physicalDeviceCount = 0;
	vkChecker(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));
	assert(physicalDeviceCount > 0);

	// Get the first discrete GPU device. Otherwise, the first device in the list.
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkChecker(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));
	mPhysicalDevice = physicalDevices.front();

	for (const VkPhysicalDevice& device : physicalDevices) {
		assert(device != VK_NULL_HANDLE);

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			mPhysicalDevice = device;
			break;
		}
	}
}
}