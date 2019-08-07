#include "PhysicalDevice.h"

#include <iostream>
#include <unordered_set>
#include <vector>

#include "DebugUtils.h"
#include "WindowSurface.h"

namespace vk {
PhysicalDevice::PhysicalDevice(const VkInstance instance, const WindowSurface& windowSurface)
	: mDeviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }
{
	assert(instance != VK_NULL_HANDLE);

	uint32_t physicalDeviceCount = 0;
	vkChecker(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));
	assert(physicalDeviceCount > 0);

	// Get all the suitable physical devices
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkChecker(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));
	std::vector<VkPhysicalDevice> suitablePhysicalDevices;
	for (const VkPhysicalDevice& device : physicalDevices) {
		assert(device != VK_NULL_HANDLE);		
		if (isPhysicalDeviceSuitable(device, windowSurface)) {
			suitablePhysicalDevices.push_back(device);
		}
	}

	assert(suitablePhysicalDevices.empty() == false && "There is no suitable physical device.");

	// From all the suitable physical devices, we get the first that is a discrete GPU.
	// Otherwise, we get the first device in the list.
	for (const VkPhysicalDevice& device : suitablePhysicalDevices) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			mPhysicalDevice = device;
			break;
		}
	}

	if (mPhysicalDevice == VK_NULL_HANDLE) {
		mPhysicalDevice = suitablePhysicalDevices.front();
	}
}

bool 
PhysicalDevice::isGraphicQueueFamilySupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t& queueFamilyIndex) {
	assert(physicalDevice != VK_NULL_HANDLE);
	assert(surface != VK_NULL_HANDLE);

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
	
	queueFamilyIndex = 0;
	for (const VkQueueFamilyProperties& queueFamilyProperty : queueFamilyProperties) {
		if (queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			return true;
		}

		++queueFamilyIndex;
	}

	return false;
}

bool
PhysicalDevice::isSurfaceSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t& queueFamilyIndex) {
	assert(physicalDevice != VK_NULL_HANDLE);
	assert(surface != VK_NULL_HANDLE);

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	queueFamilyIndex = 0;
	for (const VkQueueFamilyProperties& queueFamilyProperty : queueFamilyProperties) {
		if (queueFamilyProperty.queueCount > 0) {
			VkBool32 supported = false;
			vkChecker(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &supported));
			if (supported) {
				return true;
			}
		}

		++queueFamilyIndex;
	}

	return false;
}

bool 
PhysicalDevice::areDeviceExtensionsSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions) {
	assert(physicalDevice != VK_NULL_HANDLE);

	uint32_t extensionCount;
	vkChecker(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr));

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkChecker(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data()));
	
	std::unordered_set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const VkExtensionProperties& extensionProperty : availableExtensions) {
		requiredExtensions.erase(extensionProperty.extensionName);
	}

#ifndef NDEBUG // Debug
	for (const std::string& extensionProperty : requiredExtensions) {
		std::cerr << "Unsupported extension property: " << extensionProperty << std::endl;
	}
#endif

	return requiredExtensions.empty();
}

bool 
PhysicalDevice::isSwapChainSupported(const VkPhysicalDevice physicalDevice, const WindowSurface& windowSurface) {
	assert(physicalDevice != VK_NULL_HANDLE);
	return windowSurface.surfaceFormats(physicalDevice).empty() == false && windowSurface.presentModes(physicalDevice).empty() == false;
}

bool
PhysicalDevice::isPhysicalDeviceSuitable(const VkPhysicalDevice physicalDevice, const WindowSurface& windowSurface) {
	assert(physicalDevice != VK_NULL_HANDLE);

	return isGraphicQueueFamilySupportedByPhysicalDevice(physicalDevice, windowSurface.vkSurface(), mGraphicsSupportQueueFamilyIndex) &&
		   isSurfaceSupportedByPhysicalDevice(physicalDevice, windowSurface.vkSurface(), mSurfaceSupportQueueFamilyIndex) &&
		   areDeviceExtensionsSupportedByPhysicalDevice(physicalDevice, mDeviceExtensions) && 
		   isSwapChainSupported(physicalDevice, windowSurface);
}
}