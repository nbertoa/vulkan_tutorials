#ifndef UTILS_WINDOW
#define UTILS_WINDOW

#include <cassert>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace vk {
// GLFWwindow wrapper that provide useful methods to create/destroy/get it.
class Window {
public:
    Window(const uint32_t width, 
           const uint32_t height, 
           const char* title);
    ~Window();

    GLFWwindow& glfwWindow() const { 
        assert(mWindow != nullptr); 
        return *mWindow; 
    }

    void widthAndHeight(uint32_t& width, uint32_t& height) const;

private:
    GLFWwindow* mWindow = nullptr;
};
}

#endif