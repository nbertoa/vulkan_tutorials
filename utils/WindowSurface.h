#ifndef UTILS_WINDOW_SURFACE
#define UTILS_WINDOW_SURFACE

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace vk {
class AppInstance;
class Window;

// VkSurface wrapper to be able to create/destroy/get the vulkan surface easily.
// Since Vulkan is a platform agnostic API, it cannot interface directly with the window
// system on its own. 
// To establish the connection between Vulkan and the window system to present
// results to the screen, we need to use the WSI (Window System Integration) extensions.
// In this case, we use the VK_KHR_surface extension.
class WindowSurface {
public:
    WindowSurface(const AppInstance& appInstance,
                  Window& window);
    ~WindowSurface();
    WindowSurface(const WindowSurface&) = delete;
    const WindowSurface& operator=(const WindowSurface&) = delete;

    VkSurfaceKHR vkSurface() const { 
        assert(mSurface != VK_NULL_HANDLE); 
        return mSurface; 
    }

    VkSurfaceCapabilitiesKHR surfaceCapabilities(const VkPhysicalDevice physicalDevice) const;
    std::vector<VkSurfaceFormatKHR> surfaceFormats(const VkPhysicalDevice physicalDevice) const;
    std::vector<VkPresentModeKHR> presentModes(const VkPhysicalDevice physicalDevice) const;

private:
    const AppInstance& mAppInstance;
    VkSurfaceKHR mSurface = VK_NULL_HANDLE;
};
}

#endif