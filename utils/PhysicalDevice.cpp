#include "PhysicalDevice.h"

#include <vector>

#include "DebugUtils.h"

namespace vk {
PhysicalDevice::PhysicalDevice(const VkInstance& instance) {
	assert(instance != VK_NULL_HANDLE);

	uint32_t physicalDeviceCount = 0;
	vkChecker(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));
	assert(physicalDeviceCount > 0);

	// Get the first discrete GPU device that supports the queue families we need. 
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkChecker(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));

	for (const VkPhysicalDevice& device : physicalDevices) {
		assert(device != VK_NULL_HANDLE);

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && areQueueFamiliesSupported(device, mQueueFamilyIndex)) {
			mDevice = device;
			break;
		}
	}

	assert(mDevice != VK_NULL_HANDLE);
}

bool 
PhysicalDevice::areQueueFamiliesSupported(const VkPhysicalDevice& device, uint32_t& queueFamilyIndex) {
	assert(device != VK_NULL_HANDLE);

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

	uint32_t index = 0;
	for (const VkQueueFamilyProperties& queueFamilyProperty : queueFamilyProperties) {
		if (queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queueFamilyIndex = index;
			return true;
		}

		++index;
	}

	return false;
}
}