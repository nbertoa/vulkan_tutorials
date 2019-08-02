  #include "PhysicalDevice.h"

#include <vector>

#include "DebugUtils.h"

namespace vk {
PhysicalDevice::PhysicalDevice(const VkInstance instance, const VkSurfaceKHR surface) {
	assert(instance != VK_NULL_HANDLE);
	assert(surface != VK_NULL_HANDLE);

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

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && areQueueFamiliesSupported(device, surface, mQueueFamilyIndex)) {
			mDevice = device;
			break;
		}
	}

	assert(mDevice != VK_NULL_HANDLE);	
}

bool 
PhysicalDevice::areQueueFamiliesSupported(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t& queueFamilyIndex) {
	assert(physicalDevice != VK_NULL_HANDLE);
	assert(surface != VK_NULL_HANDLE);

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
	
	queueFamilyIndex = 0;
	for (const VkQueueFamilyProperties& queueFamilyProperty : queueFamilyProperties) {
		if (queueFamilyProperty.queueCount > 0 && 
			queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT &&
			isSurfaceSupportedByPhysicalDevice(physicalDevice, surface, queueFamilyIndex)) {
			return true;
		}

		++queueFamilyIndex;
	}

	return false;
}

bool 
PhysicalDevice::isSurfaceSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, const uint32_t queueFamilyIndex) {
	assert(physicalDevice != VK_NULL_HANDLE);
	assert(surface != VK_NULL_HANDLE);

	VkBool32 supported = false;
	vkChecker(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &supported));

	return supported;
}
}