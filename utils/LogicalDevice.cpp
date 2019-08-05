#include "LogicalDevice.h"

#include "DebugUtils.h"
#include "PhysicalDevice.h"

namespace vk {
LogicalDevice::LogicalDevice(const PhysicalDevice& physicalDevice) {
	assert(physicalDevice.vkPhysicalDevice() != VK_NULL_HANDLE);

	const std::vector<VkDeviceQueueCreateInfo> createInfoVector = buildDeviceQueueCreateInfoVector(physicalDevice);
	const std::vector<const char*>& deviceExtensions = physicalDevice.deviceExtensions();
	
	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

	VkDeviceCreateInfo physicalDeviceCreateInfo = {};
	physicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	physicalDeviceCreateInfo.pQueueCreateInfos = createInfoVector.data();
	physicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(createInfoVector.size());
	physicalDeviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
	physicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	physicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	physicalDeviceCreateInfo.enabledLayerCount = 0;
	physicalDeviceCreateInfo.ppEnabledLayerNames = nullptr;

	vkChecker(vkCreateDevice(physicalDevice.vkPhysicalDevice(), &physicalDeviceCreateInfo, nullptr, &mDevice));

	vkGetDeviceQueue(mDevice, physicalDevice.graphicsSupportQueueFamilyIndex(), 0, &mGraphicsQueue);
	assert(mGraphicsQueue != VK_NULL_HANDLE);

	vkGetDeviceQueue(mDevice, physicalDevice.surfaceSupportQueueFamilyIndex(), 0, &mPresentationQueue);
	assert(mPresentationQueue != VK_NULL_HANDLE);
}

LogicalDevice::~LogicalDevice() {
	assert(mDevice != VK_NULL_HANDLE);
	vkDestroyDevice(mDevice, nullptr);
}

std::vector<VkDeviceQueueCreateInfo> 
LogicalDevice::buildDeviceQueueCreateInfoVector(const PhysicalDevice& physicalDevice) {
	assert(physicalDevice.vkPhysicalDevice() != VK_NULL_HANDLE);

	std::vector<VkDeviceQueueCreateInfo> createInfoVector;

	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = physicalDevice.graphicsSupportQueueFamilyIndex();
		queueCreateInfo.queueCount = 1;
		const float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		createInfoVector.emplace_back(queueCreateInfo);
	}

	if (physicalDevice.graphicsSupportQueueFamilyIndex() != physicalDevice.surfaceSupportQueueFamilyIndex()) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = physicalDevice.surfaceSupportQueueFamilyIndex();
		queueCreateInfo.queueCount = 1;
		const float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		createInfoVector.emplace_back(queueCreateInfo);
	}

	return createInfoVector;
}
}