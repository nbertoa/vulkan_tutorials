#include "WindowSurface.h"

#include "DebugUtils.h"

namespace vk {
WindowSurface::WindowSurface(const VkInstance instance, 
                             GLFWwindow& glfwWindow)
    : mInstance(instance) {
    assert(mInstance != VK_NULL_HANDLE);
    vkChecker(glfwCreateWindowSurface(mInstance, 
                                      &glfwWindow, 
                                      nullptr, 
                                      &mSurface));
    assert(mSurface != VK_NULL_HANDLE);
}

WindowSurface::~WindowSurface() {
    assert(mInstance != VK_NULL_HANDLE);
    assert(mSurface != VK_NULL_HANDLE);
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
}

VkSurfaceCapabilitiesKHR
WindowSurface::surfaceCapabilities(const VkPhysicalDevice physicalDevice) const {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(mSurface != VK_NULL_HANDLE);

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkChecker(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, 
                                                        mSurface,
                                                        &surfaceCapabilities));

    return surfaceCapabilities;
}

std::vector<VkSurfaceFormatKHR>
WindowSurface::surfaceFormats(const VkPhysicalDevice physicalDevice) const {
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
WindowSurface::presentModes(const VkPhysicalDevice physicalDevice) const {
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