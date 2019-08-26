#ifndef UTILS_SURFACE
#define UTILS_SURFACE

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class AppInstance;
class Window;

// VkSurfaceKHR wrapper to be able to create/destroy/get the vulkan surface easily.
// Since Vulkan is a platform agnostic API, it cannot interface directly with the window
// system on its own. 
// To establish the connection between Vulkan and the window system to present
// results to the screen, we need to use the WSI (Window System Integration) extensions.
// In this case, we use the VK_KHR_surface extension.
// A VkSurfaceKHR object abstracts a native platform surface or window object for use 
// with Vulkan. The VK_KHR_surface extension declares the VkSurfaceKHR object, 
// and provides a function for destroying VkSurfaceKHR objects. 
// Separate platform-specific extensions each provide a function for creating a 
// VkSurfaceKHR object for the respective platform.
class Surface {
public:
    Surface(const AppInstance& appInstance,
            Window& window);
    ~Surface();
    Surface(Surface&& other) noexcept;

    Surface(const Surface&) = delete;
    const Surface& operator=(const Surface&) = delete;

    VkSurfaceKHR vkSurface() const { 
        assert(mSurface != VK_NULL_HANDLE); 
        return mSurface; 
    }

    VkSurfaceCapabilitiesKHR capabilities(const VkPhysicalDevice physicalDevice) const;
    std::vector<VkSurfaceFormatKHR> formats(const VkPhysicalDevice physicalDevice) const;
    std::vector<VkPresentModeKHR> presentModes(const VkPhysicalDevice physicalDevice) const;

private:
    const AppInstance& mAppInstance;
    VkSurfaceKHR mSurface = VK_NULL_HANDLE;
};
}

#endif