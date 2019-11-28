#include "PhysicalDeviceData.h"

#include <cassert>
#include <iostream>
#include <unordered_set>

#include "../DebugUtils.h"
#include "../Window.h"

namespace vk {
PhysicalDeviceData::PhysicalDeviceData(const VkPhysicalDevice physicalDevice,
                                       const std::vector<const char*>& deviceExtensions)
    : mPhysicalDevice(physicalDevice)
{
    assert(mPhysicalDevice != VK_NULL_HANDLE);

    mIsSupported = isGraphicQueueFamilySupported() &&
                   isTransferQueueFamilySupported() &&
                   isPresentationSupported() &&
                   areDeviceExtensionsSupported(deviceExtensions) &&
                   isSwapChainSupported() &&
                   areDeviceFeaturesSupported();
}

VkPhysicalDevice
PhysicalDeviceData::vkPhysicalDevice() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPhysicalDevice;
}

uint32_t
PhysicalDeviceData::graphicsSupportQueueFamilyIndex() const {
    assert(mIsSupported);
    return mGraphicsSupportQueueFamilyIndex;
}

uint32_t
PhysicalDeviceData::transferSupportQueueFamilyIndex() const {
    assert(mIsSupported);
    return mTransferSupportQueueFamilyIndex;
}

uint32_t
PhysicalDeviceData::presentationSupportQueueFamilyIndex() const {
    assert(mIsSupported);
    return mPresentationSupportQueueFamilyIndex;
}

bool 
PhysicalDeviceData::isSupported() const {
    return mIsSupported;
}

void
PhysicalDeviceData::queueFamilyProperties(std::vector<VkQueueFamilyProperties>& queueFamilyProperties) {
    assert(mPhysicalDevice != VK_NULL_HANDLE);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice,
                                             &queueFamilyCount,
                                             nullptr);

    queueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice,
                                             &queueFamilyCount,
                                             queueFamilyProperties.data());
}

void
PhysicalDeviceData::extensionProperties(std::vector<VkExtensionProperties>& extensionProperties) {
    assert(mPhysicalDevice != VK_NULL_HANDLE);

    uint32_t extensionCount;
    vkChecker(vkEnumerateDeviceExtensionProperties(mPhysicalDevice,
                                                   nullptr,
                                                   &extensionCount,
                                                   nullptr));

    extensionProperties.resize(extensionCount);
    vkChecker(vkEnumerateDeviceExtensionProperties(mPhysicalDevice,
                                                   nullptr,
                                                   &extensionCount,
                                                   extensionProperties.data()));
}

bool
PhysicalDeviceData::isGraphicQueueFamilySupported() {
    std::vector<VkQueueFamilyProperties> properties;
    queueFamilyProperties(properties);

    mGraphicsSupportQueueFamilyIndex = 0;
    for (const VkQueueFamilyProperties& queueFamilyProperty : properties) {
        if (queueFamilyProperty.queueCount > 0 &&
            queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return true;
        }

        ++mGraphicsSupportQueueFamilyIndex;
    }

    return false;
}

bool
PhysicalDeviceData::isTransferQueueFamilySupported() {
    std::vector<VkQueueFamilyProperties> properties;
    queueFamilyProperties(properties);

    mTransferSupportQueueFamilyIndex = 0;
    for (const VkQueueFamilyProperties& queueFamilyProperty : properties) {
        if (queueFamilyProperty.queueCount > 0 &&
            queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            return true;
        }

        ++mTransferSupportQueueFamilyIndex;
    }

    return false;
}

bool
PhysicalDeviceData::isPresentationSupported() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);

    std::vector<VkQueueFamilyProperties> properties;
    queueFamilyProperties(properties);

    mPresentationSupportQueueFamilyIndex = 0;
    for (const VkQueueFamilyProperties& queueFamilyProperty : properties) {
        if (queueFamilyProperty.queueCount > 0) {
            if (Window::isPhysicalDeviceSupported(mPhysicalDevice,
                                                  mPresentationSupportQueueFamilyIndex)) {
                return true;
            }
        }

        ++mPresentationSupportQueueFamilyIndex;
    }

    return false;
}

bool
PhysicalDeviceData::areDeviceExtensionsSupported(const std::vector<const char*>& deviceExtensions) {
    std::vector<VkExtensionProperties> properties;
    extensionProperties(properties);

    std::unordered_set<std::string> requiredExtensions(deviceExtensions.begin(),
                                                       deviceExtensions.end());

    for (const VkExtensionProperties& extensionProperty : properties) {
        requiredExtensions.erase(extensionProperty.extensionName);
    }

#ifdef _DEBUG
    for (const std::string& extensionProperty : requiredExtensions) {
        std::cerr << "Unsupported extension property: " << extensionProperty << std::endl;
    }
#endif

    return requiredExtensions.empty();
}

bool
PhysicalDeviceData::isSwapChainSupported() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return Window::physicalDeviceSurfaceFormats(mPhysicalDevice).empty() == false &&
           Window::physicalDeviceSurfacePresentModes(mPhysicalDevice).empty() == false;
}

bool
PhysicalDeviceData::areDeviceFeaturesSupported() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
   
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(mPhysicalDevice,
                                &supportedFeatures);

    return supportedFeatures.samplerAnisotropy;
}
}