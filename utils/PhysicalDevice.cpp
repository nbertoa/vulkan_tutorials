#include "PhysicalDevice.h"

#include <iostream>
#include <unordered_set>
#include <vector>

#include "AppInstance.h"
#include "DebugUtils.h"
#include "Surface.h"

namespace vk {
PhysicalDevice::PhysicalDevice(const AppInstance& appInstance,
                               const Surface& surface)
    : mDeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME} 
{
    setPhysicalDevice(getCandidateDevices(appInstance,
                                          surface));
}

bool
PhysicalDevice::isGraphicQueueFamilySupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                              uint32_t& queueFamilyIndex) {
    assert(physicalDevice != VK_NULL_HANDLE);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, 
                                             &queueFamilyCount, 
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, 
                                             &queueFamilyCount, 
                                             queueFamilyProperties.data());

    queueFamilyIndex = 0;
    for (const VkQueueFamilyProperties& queueFamilyProperty : queueFamilyProperties) {
        if (queueFamilyProperty.queueCount > 0 && 
            queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return true;
        }

        ++queueFamilyIndex;
    }

    return false;
}

bool
PhysicalDevice::isPresentationSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                        const VkSurfaceKHR surface, 
                                                        uint32_t& queueFamilyIndex) {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(surface != VK_NULL_HANDLE);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, 
                                             &queueFamilyCount, 
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, 
                                             &queueFamilyCount, 
                                             queueFamilyProperties.data());

    queueFamilyIndex = 0;
    for (const VkQueueFamilyProperties& queueFamilyProperty : queueFamilyProperties) {
        if (queueFamilyProperty.queueCount > 0) {
            VkBool32 supported = false;
            vkChecker(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, 
                                                           queueFamilyIndex, 
                                                           surface,
                                                           &supported));
            if (supported) {
                return true;
            }
        }

        ++queueFamilyIndex;
    }

    return false;
}

bool
PhysicalDevice::areDeviceExtensionsSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                             const std::vector<const char*>& deviceExtensions) {
    assert(physicalDevice != VK_NULL_HANDLE);

    uint32_t extensionCount;
    vkChecker(vkEnumerateDeviceExtensionProperties(physicalDevice, 
                                                   nullptr,
                                                   &extensionCount,
                                                   nullptr));

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkChecker(vkEnumerateDeviceExtensionProperties(physicalDevice, 
                                                   nullptr,
                                                   &extensionCount, 
                                                   availableExtensions.data()));

    std::unordered_set<std::string> requiredExtensions(deviceExtensions.begin(), 
                                                       deviceExtensions.end());

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
PhysicalDevice::isSwapChainSupported(const VkPhysicalDevice physicalDevice, 
                                     const Surface& surface) {
    assert(physicalDevice != VK_NULL_HANDLE);
    return surface.formats(physicalDevice).empty() == false && 
           surface.presentModes(physicalDevice).empty() == false;
}

bool
PhysicalDevice::isPhysicalDeviceSuitable(const VkPhysicalDevice physicalDevice, 
                                         const Surface& surface,
                                         uint32_t& graphicsSupportQueueFamilyIndex,
                                         uint32_t& presentationSupportQueueFamilyIndex) const {
    assert(physicalDevice != VK_NULL_HANDLE);

    return isGraphicQueueFamilySupportedByPhysicalDevice(physicalDevice, 
                                                         graphicsSupportQueueFamilyIndex) &&
           isPresentationSupportedByPhysicalDevice(physicalDevice, 
                                                   surface.vkSurface(), 
                                                   presentationSupportQueueFamilyIndex) &&
           areDeviceExtensionsSupportedByPhysicalDevice(physicalDevice, 
                                                        mDeviceExtensions) &&
           isSwapChainSupported(physicalDevice, 
                                surface);
}

std::vector<PhysicalDevice::PhysicalDeviceInfo>
PhysicalDevice::getCandidateDevices(const AppInstance& appInstance,
                                    const Surface& surface) const {
    uint32_t physicalDeviceCount = 0;
    vkChecker(vkEnumeratePhysicalDevices(appInstance.vkInstance(), 
                                         &physicalDeviceCount, 
                                         nullptr));
    assert(physicalDeviceCount > 0);

    // Get all the suitable physical devices
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkChecker(vkEnumeratePhysicalDevices(appInstance.vkInstance(),
                                         &physicalDeviceCount,
                                         physicalDevices.data()));
    std::vector<PhysicalDeviceInfo> candidatePhysicalDevices;
    for (const VkPhysicalDevice& device : physicalDevices) {
        assert(device != VK_NULL_HANDLE);

        uint32_t graphicsSupportQueueFamilyIndex;
        uint32_t presentationSupportQueueFamilyIndex;
        if (isPhysicalDeviceSuitable(device, 
                                     surface,
                                     graphicsSupportQueueFamilyIndex,
                                     presentationSupportQueueFamilyIndex)) {
            PhysicalDeviceInfo physicalDeviceInfo;
            physicalDeviceInfo.mDevice = device;
            physicalDeviceInfo.mGraphicsSupportQueueFamilyIndex = graphicsSupportQueueFamilyIndex;
            physicalDeviceInfo.mPresentationSupportQueueFamilyIndex = presentationSupportQueueFamilyIndex;
            candidatePhysicalDevices.push_back(physicalDeviceInfo);
        }
    }

    return candidatePhysicalDevices;
}

void
PhysicalDevice::setPhysicalDevice(const std::vector<PhysicalDevice::PhysicalDeviceInfo>& candidateDevices) {
    assert(candidateDevices.empty() == false && "There is no suitable physical device.");
    assert(mPhysicalDevice == VK_NULL_HANDLE);

    // From all the suitable physical devices, we get the first that is a discrete GPU.
    // Otherwise, we get the first device in the list.
    for (const PhysicalDeviceInfo& deviceInfo : candidateDevices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(deviceInfo.mDevice, &deviceProperties);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            mPhysicalDevice = deviceInfo.mDevice;
            mGraphicsSupportQueueFamilyIndex = deviceInfo.mGraphicsSupportQueueFamilyIndex;
            mPresentationSupportQueueFamilyIndex = deviceInfo.mPresentationSupportQueueFamilyIndex;
            break;
        }
    }

    if (mPhysicalDevice == VK_NULL_HANDLE) {
        const PhysicalDeviceInfo& deviceInfo = candidateDevices.front();
        mPhysicalDevice = deviceInfo.mDevice;
        mGraphicsSupportQueueFamilyIndex = deviceInfo.mGraphicsSupportQueueFamilyIndex;
        mPresentationSupportQueueFamilyIndex = deviceInfo.mPresentationSupportQueueFamilyIndex;
    }
}
}