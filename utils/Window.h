#ifndef UTILS_WINDOW
#define UTILS_WINDOW

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace vk {
// GLFWwindow wrapper.
// The GLFWwindow object encapsulates both a window and a context.
// As the window and context are inseparably linked, the object 
// pointer is used as both a context and window handle.
class Window {
public:
    Window(const uint32_t width, 
           const uint32_t height, 
           const char* title);
    ~Window();
    Window(Window&& other) noexcept;
    Window(const Window&) = delete;
    const Window& operator=(const Window&) = delete;

    GLFWwindow& glfwWindow() const;

    void widthAndHeight(uint32_t& width, uint32_t& height) const;
    uint32_t width() const;
    uint32_t height() const;

private:
    GLFWwindow* mWindow = nullptr;
};
}

#endif