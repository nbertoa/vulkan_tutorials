#include "Window.h"

#include "DebugUtils.h"
#include "Instance.h"

namespace vk2 {
GLFWwindow* 
Window::mWindow = nullptr;

VkSurfaceKHR 
Window::mSurface = VK_NULL_HANDLE;

void
Window::initialize(const uint32_t width, 
                   const uint32_t height,
                   const char* title) {
    assert(width > 0);
    assert(height > 0);
    assert(title != nullptr);
    assert(mWindow == nullptr);
    assert(mSurface == VK_NULL_HANDLE);
    
    // Avoid OpenGL context creation
    glfwWindowHint(GLFW_CLIENT_API, 
                   GLFW_NO_API);

    // Avoid window resizing
    glfwWindowHint(GLFW_RESIZABLE, 
                   GLFW_FALSE);

    mWindow = glfwCreateWindow(static_cast<int>(width), 
                               static_cast<int>(height), 
                               title, 
                               nullptr, 
                               nullptr);
    assert(mWindow != nullptr && "Window creation failed");

    vkChecker(glfwCreateWindowSurface(Instance::vkInstance(),
                                      mWindow,
                                      nullptr,
                                      &mSurface));
}

void
Window::finalize() {
    assert(mSurface != VK_NULL_HANDLE);
    vkDestroySurfaceKHR(Instance::vkInstance(),
                        mSurface,
                        nullptr);

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

VkSurfaceKHR
Window::vkSurface() {
    assert(mSurface != VK_NULL_HANDLE);
    return mSurface;
}

bool 
Window::shouldCloseWindow() {
    assert(mWindow != nullptr);

    return glfwWindowShouldClose(mWindow) != 0;
}

void
Window::widthAndHeight(uint32_t& width, 
                       uint32_t& height) {
    assert(mWindow != nullptr);
    int w;
    int h;
    glfwGetWindowSize(mWindow, &w, &h);
    width = static_cast<uint32_t>(w);
    height = static_cast<uint32_t>(h);
}

uint32_t
Window::width() {
    assert(mWindow != nullptr);
    int w;
    int h;
    glfwGetWindowSize(mWindow, 
                      &w,
                      &h);
    return static_cast<uint32_t>(w);
}

uint32_t
Window::height() {
    assert(mWindow != nullptr);
    int w;
    int h;
    glfwGetWindowSize(mWindow, 
                      &w, 
                      &h);
    return static_cast<uint32_t>(h);
}

VkSurfaceCapabilitiesKHR
Window::physicalDeviceSurfaceCapabilities(const VkPhysicalDevice physicalDevice) {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(mSurface != VK_NULL_HANDLE);

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkChecker(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,
                                                        mSurface,
                                                        &surfaceCapabilities));

    return surfaceCapabilities;
}

std::vector<VkSurfaceFormatKHR>
Window::physicalDeviceSurfaceFormats(const VkPhysicalDevice physicalDevice) {
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
Window::physicalDeviceSurfacePresentModes(const VkPhysicalDevice physicalDevice) {
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
Window::isPresentationSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice,
                                                const uint32_t queueFamilyIndex) {
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