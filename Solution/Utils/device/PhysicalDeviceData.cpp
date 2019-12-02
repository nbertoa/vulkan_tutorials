#include "PhysicalDeviceData.h"

#include <cassert>
#include <iostream>
#include <unordered_set>

#include "../DebugUtils.h"
#include "../Window.h"

namespace vk2 {
PhysicalDeviceData::PhysicalDeviceData(const vk::PhysicalDevice physicalDevice,
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

vk::PhysicalDevice
PhysicalDeviceData::device() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPhysicalDevice;
}

uint32_t
PhysicalDeviceData::graphicsQueueFamilyIndex() const {
    assert(mIsSupported);
    return mGraphicsQueueFamilyIndex;
}

uint32_t
PhysicalDeviceData::transferQueueFamilyIndex() const {
    assert(mIsSupported);
    return mTransferQueueFamilyIndex;
}

uint32_t
PhysicalDeviceData::presentationQueueFamilyIndex() const {
    assert(mIsSupported);
    return mPresentationQueueFamilyIndex;
}

bool 
PhysicalDeviceData::isSupported() const {
    return mIsSupported;
}

bool
PhysicalDeviceData::isGraphicQueueFamilySupported() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);

    const std::vector<vk::QueueFamilyProperties> properties =
        mPhysicalDevice.getQueueFamilyProperties();

    mGraphicsQueueFamilyIndex = 0;
    for (const vk::QueueFamilyProperties& queueFamilyProperty : properties) {
        if (queueFamilyProperty.queueCount > 0 &&
            queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics) {
            return true;
        }

        ++mGraphicsQueueFamilyIndex;
    }

    return false;
}

bool
PhysicalDeviceData::isTransferQueueFamilySupported() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);

    const std::vector<vk::QueueFamilyProperties> properties =
        mPhysicalDevice.getQueueFamilyProperties();

    mTransferQueueFamilyIndex = 0;
    for (const vk::QueueFamilyProperties& queueFamilyProperty : properties) {
        if (queueFamilyProperty.queueCount > 0 &&
            queueFamilyProperty.queueFlags & vk::QueueFlagBits::eTransfer) {
            return true;
        }

        ++mTransferQueueFamilyIndex;
    }

    return false;
}

bool
PhysicalDeviceData::isPresentationSupported() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);

    const std::vector<vk::QueueFamilyProperties> properties =
        mPhysicalDevice.getQueueFamilyProperties();

    mPresentationQueueFamilyIndex = 0;
    for (const VkQueueFamilyProperties& queueFamilyProperty : properties) {
        if (queueFamilyProperty.queueCount > 0) {
            if (Window::isPresentationSupportedByPhysicalDevice(mPhysicalDevice,
                                                                mPresentationQueueFamilyIndex)) {
                return true;
            }
        }

        ++mPresentationQueueFamilyIndex;
    }

    return false;
}

bool
PhysicalDeviceData::areDeviceExtensionsSupported(const std::vector<const char*>& deviceExtensions) {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    
    const std::vector<vk::ExtensionProperties> properties =
        mPhysicalDevice.enumerateDeviceExtensionProperties();

    std::unordered_set<std::string> requiredExtensions(deviceExtensions.begin(),
                                                       deviceExtensions.end());

    for (const vk::ExtensionProperties& extensionProperty : properties) {
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

    return mPhysicalDevice.getFeatures().samplerAnisotropy;
}
}