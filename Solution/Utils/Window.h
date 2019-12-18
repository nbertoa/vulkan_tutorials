#ifndef UTILS_WINDOW
#define UTILS_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vulkan {
//
// GLFWwindow & SurfaceKHR wrapper.
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
// A SurfaceKHR object abstracts a native platform surface or window object for use 
// with Vulkan. 
//
// The VK_KHR_surface extension declares the vk::SurfaceKHR object, and provides a function 
// for destroying vk::SurfaceKHR objects. 
//
// Separate platform-specific extensions each provide a function for creating a 
// vk::SurfaceKHR object for the respective platform.
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

    static vk::SurfaceKHR
    surface();
    
    static bool 
    shouldCloseWindow();

    static void
    widthAndHeight(uint32_t& width, 
                   uint32_t& height);

    static uint32_t
    width();

    static uint32_t
    height();

private:
    static GLFWwindow* mWindow;
    static vk::SurfaceKHR mSurface;
};
}

#endif