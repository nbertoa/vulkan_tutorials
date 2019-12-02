#ifndef UTILS_DEVICE_PHYSICAL_DEVICE_DATA
#define UTILS_DEVICE_PHYSICAL_DEVICE_DATA

#include <vector>
#include <vulkan/vulkan.hpp>

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
    PhysicalDeviceData(const vk::PhysicalDevice physicalDevice,
                       const std::vector<const char*>& deviceExtensions);

    vk::PhysicalDevice
    device() const;

    uint32_t
    graphicsQueueFamilyIndex() const;

    uint32_t
    transferQueueFamilyIndex() const;

    uint32_t
    presentationQueueFamilyIndex() const;

    bool
    isSupported() const;

private:
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
    
    vk::PhysicalDevice mPhysicalDevice;
    uint32_t mGraphicsQueueFamilyIndex = 0;
    uint32_t mTransferQueueFamilyIndex = 0;
    uint32_t mPresentationQueueFamilyIndex = 0;
    bool mIsSupported = false;
};
}

#endif