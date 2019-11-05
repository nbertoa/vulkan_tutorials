#include "Surface.h"

#include <cassert>

#include "DebugUtils.h"
#include "Instance.h"
#include "Window.h"
#include "device/PhysicalDevice.h"

namespace vk {
Surface::Surface(const Instance& instance,
                 const Window& window)
    : mInstance(instance)
{
    mSurface =  window.createSurfaceForWindow(mInstance);
}

Surface::~Surface() {
    assert(mSurface != VK_NULL_HANDLE);
    vkDestroySurfaceKHR(mInstance.vkInstance(), mSurface, nullptr);
}

Surface::Surface(Surface&& other) noexcept
    : mInstance(other.mInstance)
    , mSurface(other.mSurface)
{
    other.mSurface = VK_NULL_HANDLE;
}

VkSurfaceKHR 
Surface::vkSurface() const {
    assert(mSurface != VK_NULL_HANDLE);
    return mSurface;
}

VkSurfaceCapabilitiesKHR
Surface::physicalDeviceSurfaceCapabilities(const VkPhysicalDevice physicalDevice) const {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(mSurface != VK_NULL_HANDLE);

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkChecker(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, 
                                                        mSurface,
                                                        &surfaceCapabilities));

    return surfaceCapabilities;
}

std::vector<VkSurfaceFormatKHR>
Surface::physicalDeviceSurfaceFormats(const VkPhysicalDevice physicalDevice) const {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(mSurface != VK_NULL_HANDLE);
    
    uint32_t surfaceFormatCount;
    vkChecker(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, 
                                                   mSurface, 
                                                   &surfaceFormatCount, 
                                                   nullptr));

    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    if (surfaceFormatCount > 0) {
        surfaceFormats.resize(surfaceFormatCount);
        vkChecker(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, 
                                                       mSurface, 
                                                       &surfaceFormatCount, 
                                                       surfaceFormats.data()));
    }

    return surfaceFormats;
}

std::vector<VkPresentModeKHR>
Surface::physicalDeviceSurfacePresentModes(const VkPhysicalDevice physicalDevice) const {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(mSurface != VK_NULL_HANDLE);    

    uint32_t presentModeCount;
    vkChecker(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, 
                                                        mSurface, 
                                                        &presentModeCount, 
                                                        nullptr));

    std::vector<VkPresentModeKHR> presentModes;
    if (presentModeCount > 0) {
        presentModes.resize(presentModeCount);
        vkChecker(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, 
                                                            mSurface, 
                                                            &presentModeCount, 
                                                            presentModes.data()));
    }

    return presentModes;
}

bool 
Surface::isPhysicalDeviceSupported(const VkPhysicalDevice physicalDevice,
                                   const uint32_t queueFamilyIndex) const {
    assert(mSurface != VK_NULL_HANDLE);
    assert(physicalDevice != VK_NULL_HANDLE);

    VkBool32 supported = false;
    vkChecker(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,
                                                   queueFamilyIndex,
                                                   mSurface,
                                                   &supported));

    return supported;
}
}