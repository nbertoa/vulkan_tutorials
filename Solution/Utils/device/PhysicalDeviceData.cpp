#include "PhysicalDeviceData.h"

#include <cassert>
#include <iostream>
#include <unordered_set>

#include "../Window.h"

namespace vulkan {
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
    
    mGraphicsQueueFamilyIndex = 0;
    for (const vk::QueueFamilyProperties& property : 
         mPhysicalDevice.getQueueFamilyProperties()) {
        if (property.queueCount > 0 &&
            property.queueFlags & vk::QueueFlagBits::eGraphics) {
            return true;
        }

        ++mGraphicsQueueFamilyIndex;
    }

    return false;
}

bool
PhysicalDeviceData::isTransferQueueFamilySupported() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    
    mTransferQueueFamilyIndex = 0;
    for (const vk::QueueFamilyProperties& property : 
         mPhysicalDevice.getQueueFamilyProperties()) {
        if (property.queueCount > 0 &&
            property.queueFlags & vk::QueueFlagBits::eTransfer) {
            return true;
        }

        ++mTransferQueueFamilyIndex;
    }

    return false;
}

bool
PhysicalDeviceData::isPresentationSupported() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);

    mPresentationQueueFamilyIndex = 0;
    for (const vk::QueueFamilyProperties& property : 
         mPhysicalDevice.getQueueFamilyProperties()) {
        if (property.queueCount > 0) {
            if (mPhysicalDevice.getSurfaceSupportKHR(mPresentationQueueFamilyIndex,
                                                     Window::surface())) {
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
    
    std::unordered_set<std::string> requiredExtensions(deviceExtensions.begin(),
                                                       deviceExtensions.end());

    for (const vk::ExtensionProperties& property : 
         mPhysicalDevice.enumerateDeviceExtensionProperties()) {
        requiredExtensions.erase(property.extensionName);
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
    return mPhysicalDevice.getSurfaceFormatsKHR(Window::surface()).empty() == false &&
           mPhysicalDevice.getSurfacePresentModesKHR(Window::surface()).empty() == false;
}

bool
PhysicalDeviceData::areDeviceFeaturesSupported() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPhysicalDevice.getFeatures().samplerAnisotropy;
}
}