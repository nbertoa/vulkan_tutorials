#include "Surface.h"

#include "AppInstance.h"
#include "DebugUtils.h"
#include "PhysicalDevice.h"
#include "Window.h"

namespace vk {
Surface::Surface(const AppInstance& appInstance,
                       Window& window)
    : mAppInstance(appInstance) 
{
    vkChecker(glfwCreateWindowSurface(mAppInstance.vkInstance(),
                                      &window.glfwWindow(),
                                      nullptr, 
                                      &mSurface));
    assert(mSurface != VK_NULL_HANDLE);
}

Surface::~Surface() {
    assert(mSurface != VK_NULL_HANDLE);
    vkDestroySurfaceKHR(mAppInstance.vkInstance(), mSurface, nullptr);
}

Surface::Surface(Surface&& other) noexcept
    : mAppInstance(other.mAppInstance)
    , mSurface(other.mSurface)
{
    other.mSurface = VK_NULL_HANDLE;
}

VkSurfaceCapabilitiesKHR
Surface::capabilities(const VkPhysicalDevice physicalDevice) const {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(mSurface != VK_NULL_HANDLE);

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkChecker(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, 
                                                        mSurface,
                                                        &surfaceCapabilities));

    return surfaceCapabilities;
}

std::vector<VkSurfaceFormatKHR>
Surface::formats(const VkPhysicalDevice physicalDevice) const {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(mSurface != VK_NULL_HANDLE);

    std::vector<VkSurfaceFormatKHR> surfaceFormats;

    uint32_t surfaceFormatCount;
    vkChecker(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, 
                                                   mSurface, 
                                                   &surfaceFormatCount, 
                                                   nullptr));

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
Surface::presentModes(const VkPhysicalDevice physicalDevice) const {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(mSurface != VK_NULL_HANDLE);

    std::vector<VkPresentModeKHR> presentModes;

    uint32_t presentModeCount;
    vkChecker(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, 
                                                        mSurface, 
                                                        &presentModeCount, 
                                                        nullptr));

    if (presentModeCount > 0) {
        presentModes.resize(presentModeCount);
        vkChecker(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, 
                                                            mSurface, 
                                                            &presentModeCount, 
                                                            presentModes.data()));
    }

    return presentModes;
}
}