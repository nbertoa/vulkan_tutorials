#ifndef UTILS_WINDOW
#define UTILS_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

namespace vk2 {
//
// GLFWwindow & VkSurfaceKHR wrapper.
//
// The GLFWwindow object encapsulates both a window and a context.
// 
// As the window and context are inseparably linked, the object 
// pointer is used as both a context and window handle.
//
// Since Vulkan is a platform agnostic API, it cannot interface directly with the window
// system on its own. 
//
// To establish the connection between Vulkan and the window system to present
// results to the screen, we need to use the WSI (Window System Integration) extensions.
//
// A VkSurfaceKHR object abstracts a native platform surface or window object for use 
// with Vulkan. 
//
// The VK_KHR_surface extension declares the VkSurfaceKHR object, and provides a function 
// for destroying VkSurfaceKHR objects. 
//
// Separate platform-specific extensions each provide a function for creating a 
// VkSurfaceKHR object for the respective platform.
//
// You need the Window to:
// - Create the surface
// - Create the SwapChain
//
class Window {
public:
    Window() = delete;
    ~Window() = delete;
    Window(Window&& other) = delete;
    Window(const Window&) = delete;
    const Window& operator=(const Window&) = delete;

    // Preconditions:
    // - GLFW must be initialized first.
    static void
    initialize(const uint32_t width,
               const uint32_t height,
               const char* title);

    static void
    finalize();

    static VkSurfaceKHR
    vkSurface();
    
    static bool 
    shouldCloseWindow();

    static void
    widthAndHeight(uint32_t& width, 
                   uint32_t& height);

    static uint32_t
    width();

    static uint32_t
    height();

    // Window's surface capabilities supported by the physical device that
    // needed to create the swap chain
    static VkSurfaceCapabilitiesKHR
    physicalDeviceSurfaceCapabilities(const VkPhysicalDevice physicalDevice);

    // Window's surface color formats supported by the physical device that 
    // are needed to create the swap chain
    static std::vector<VkSurfaceFormatKHR>
    physicalDeviceSurfaceFormats(const VkPhysicalDevice physicalDevice);

    // Window's surface presentation modes supported by the physical device
    // that are needed to create the swap chain
    static std::vector<VkPresentModeKHR>
    physicalDeviceSurfacePresentModes(const VkPhysicalDevice physicalDevice);

    // Returns true if presentation on window's surface is supported
    // by the physicalDevice
    static bool
    isPresentationSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice,
                                            const uint32_t queueFamilyIndex);

private:
    static GLFWwindow* mWindow;
    static VkSurfaceKHR mSurface;
};
}

#endif