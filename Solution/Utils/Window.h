#ifndef UTILS_WINDOW
#define UTILS_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vk {
class Instance;

//
// GLFWwindow wrapper.
//
// The GLFWwindow object encapsulates both a window and a context.
// 
// As the window and context are inseparably linked, the object 
// pointer is used as both a context and window handle.
//
// You need the Window to:
// - Create the Surface
//
class Window {
public:
    Window(const uint32_t width, 
           const uint32_t height, 
           const char* title);
    ~Window();
    Window(Window&& other) noexcept;
    Window(const Window&) = delete;
    const Window& operator=(const Window&) = delete;

    // * instance to create the surface in.
    VkSurfaceKHR
    createSurfaceForWindow(const Instance& instance) const;

    bool 
    shouldCloseWindow() const;

    void 
    widthAndHeight(uint32_t& width, 
                   uint32_t& height) const;

    uint32_t 
    width() const;

    uint32_t 
    height() const;

private:
    GLFWwindow* mWindow = nullptr;
};
}

#endif