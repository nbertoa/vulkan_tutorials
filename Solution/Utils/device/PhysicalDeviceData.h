#ifndef UTILS_DEVICE_PHYSICAL_DEVICE_DATA
#define UTILS_DEVICE_PHYSICAL_DEVICE_DATA

#include <vector>
#include <vulkan/vulkan.h>

namespace vk2 {
//
// Useful data for the PhysicalDevice which includes
// the vulkan device and queue family indices.
//
// The responsibility of this class is to check if the
// physical device supports the surface, device extensions,
// and the different queue families.
//
// This class is used internally by PhysicalDevice to 
// get information about all the candidate physical devices
// and choose the best fit.
//
class PhysicalDeviceData {
public:
    // * deviceExtensions contains the extensions that the device must support.
    //   Extensions may define new Vulkan commands, structures, and enumerants.
    //   Extensions may extend or change the behavior of the Vulkan API.
    PhysicalDeviceData(const VkPhysicalDevice physicalDevice,
                       const std::vector<const char*>& deviceExtensions);

    VkPhysicalDevice
    vkPhysicalDevice() const;

    uint32_t
    graphicsSupportQueueFamilyIndex() const;

    uint32_t
    transferSupportQueueFamilyIndex() const;

    uint32_t
    presentationSupportQueueFamilyIndex() const;

    bool
    isSupported() const;

private:
    // Queue family properties like number of queues in the family
    // and capabilities (grapphics, compute, etc)
    void
    queueFamilyProperties(std::vector<VkQueueFamilyProperties>& queueFamilyProperties);

    void
    extensionProperties(std::vector<VkExtensionProperties>& extensionProperties);

    bool
    isGraphicQueueFamilySupported();

    bool
    isTransferQueueFamilySupported();

    bool
    isPresentationSupported();

    bool
    areDeviceExtensionsSupported(const std::vector<const char*>& deviceExtensions);

    bool 
    isSwapChainSupported() const;

    bool
    areDeviceFeaturesSupported() const;
    
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    uint32_t mGraphicsSupportQueueFamilyIndex = 0;
    uint32_t mTransferSupportQueueFamilyIndex = 0;
    uint32_t mPresentationSupportQueueFamilyIndex = 0;
    bool mIsSupported = false;
};
}

#endif