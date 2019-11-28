#ifndef UTILS_WINDOW
#define UTILS_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

namespace vk {
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
// - Create the Surface
// - Create the SwapChain
// - Get the candidate devices to create the PhysicalDevice
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

    // This is needed to create the SwapChain
    // VkSurfaceCapabilitiesKHR:
    // - physicalDevice that will be associated with the swapchain to be created, 
    //   as described for vkCreateSwapchainKHR.
    // - surface that will be associated with the swapchain.
    // - pSurfaceCapabilities array in which the capabilities are returned.
    static VkSurfaceCapabilitiesKHR
    physicalDeviceSurfaceCapabilities(const VkPhysicalDevice physicalDevice);

    // This is needed to create the SwapChain
    // VkSurfaceFormatKHR:
    // - format that is compatible with the specified surface.
    // - colorSpace that is compatible with the surface.
    static std::vector<VkSurfaceFormatKHR>
    physicalDeviceSurfaceFormats(const VkPhysicalDevice physicalDevice);

    // This is needed to create the SwapChain
    // VkPresentModeKHR:
    // - IMMEDIATE_KHR, MAILBOX_KHR, FIFO_KHR, FIFO_RELAXED_KHR, SHARED_DEMAND_REFRESH_KHR, 
    //   SHARED_CONTINUOUS_REFRESH_KHR
    //
    //   The application is only required to make one initial presentation request, 
    //   after which the presentation engine must update the current image without any need for 
    //   further presentation requests.
    //   The application can indicate the image contents have been updated by making a presentation 
    //   request, but this does not guarantee the timing of when it will be updated.
    //   This mode may result in visible tearing if rendering to the image is not timed correctly.
    static std::vector<VkPresentModeKHR>
    physicalDeviceSurfacePresentModes(const VkPhysicalDevice physicalDevice);

    // This is used to determine whether a queue family of a physical device 
    // supports presentation to a given surface.
    static bool
    isPhysicalDeviceSupported(const VkPhysicalDevice physicalDevice,
                              const uint32_t queueFamilyIndex);

private:
    static GLFWwindow* mWindow;
    static VkSurfaceKHR mSurface;
};
}

#endif