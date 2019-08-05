#ifndef UTILS_PHYSICAL_DEVICE
#define UTILS_PHYSICAL_DEVICE

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
// VkPhysicalDevice wrapper to be able to create/destroy/get it easily.
// It represents a physical device installed in the system.
class PhysicalDevice {
public:
	PhysicalDevice(const VkInstance instance, const VkSurfaceKHR surface);

	VkPhysicalDevice vkPhysicalDevice() const { assert(mDevice != VK_NULL_HANDLE); return mDevice; }
	uint32_t graphicsSupportQueueFamilyIndex() const { assert(mDevice != VK_NULL_HANDLE); return mGraphicsSupportQueueFamilyIndex; }
	uint32_t surfaceSupportQueueFamilyIndex() const { assert(mDevice != VK_NULL_HANDLE); return mSurfaceSupportQueueFamilyIndex; }
	const std::vector<const char*>& deviceExtensions() const { assert(mDevice != VK_NULL_HANDLE); return mDeviceExtensions; }
	
private:
	static bool isGraphicQueueFamilySupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t& queueFamilyIndex);

	static bool isSurfaceSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t& queueFamilyIndex);

	static bool areDeviceExtensionsSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions);

	VkPhysicalDevice mDevice = VK_NULL_HANDLE;

	uint32_t mGraphicsSupportQueueFamilyIndex = 0;
	uint32_t mSurfaceSupportQueueFamilyIndex = 0;

	std::vector<const char*> mDeviceExtensions;
};
}

#endif