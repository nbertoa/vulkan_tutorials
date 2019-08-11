#include "Window.h"

#include "DebugUtils.h"

namespace vk {
Window::Window(const uint32_t width, 
               const uint32_t height, 
               const char* title) {
    assert(width > 0);
    assert(height > 0);
    assert(title != nullptr);

    glfwChecker(glfwInit());

    // Avoid OpenGL context creation
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Avoid window resizing
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mWindow = glfwCreateWindow(static_cast<int>(width), 
                               static_cast<int>(height), 
                               title, 
                               nullptr, 
                               nullptr);
    assert(mWindow != nullptr && "Window creation failed");
}

Window::~Window() {
    assert(mWindow != nullptr);
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void
Window::widthAndHeight(uint32_t& width, uint32_t& height) const {
    assert(mWindow != nullptr);
    int w;
    int h;
    glfwGetWindowSize(mWindow, &w, &h);
    width = static_cast<uint32_t>(w);
    height = static_cast<uint32_t>(h);
}
}