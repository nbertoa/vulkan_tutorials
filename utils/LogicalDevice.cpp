#include "LogicalDevice.h"

#include "DebugUtils.h"
#include "PhysicalDevice.h"

namespace vk {
LogicalDevice::LogicalDevice(const PhysicalDevice& physicalDevice) {
	assert(physicalDevice.vkPhysicalDevice() != VK_NULL_HANDLE);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = physicalDevice.queueFamilyIndex();
	queueCreateInfo.queueCount = 1;
	const float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

	VkDeviceCreateInfo physicalDeviceCreateInfo = {};
	physicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	physicalDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	physicalDeviceCreateInfo.queueCreateInfoCount = 1;
	physicalDeviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
	physicalDeviceCreateInfo.enabledExtensionCount = 0;
	physicalDeviceCreateInfo.ppEnabledExtensionNames = nullptr;
	physicalDeviceCreateInfo.enabledLayerCount = 0;
	physicalDeviceCreateInfo.ppEnabledLayerNames = nullptr;

	vkChecker(vkCreateDevice(physicalDevice.vkPhysicalDevice(), &physicalDeviceCreateInfo, nullptr, &mDevice));

	vkGetDeviceQueue(mDevice, physicalDevice.queueFamilyIndex(), 0, &mGraphicsQueue);
	assert(mGraphicsQueue != VK_NULL_HANDLE);
}

LogicalDevice::~LogicalDevice() {
	assert(mDevice != VK_NULL_HANDLE);
	vkDestroyDevice(mDevice, nullptr);
}
}