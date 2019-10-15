#include "Window.h"

#include "DebugUtils.h"
#include "Instance.h"

namespace vk {
Window::Window(const uint32_t width, 
               const uint32_t height, 
               const char* title) {
    assert(width > 0);
    assert(height > 0);
    assert(title != nullptr);

    glfwChecker(glfwInit());

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
}

Window::~Window() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

Window::Window(Window&& other) noexcept
    : mWindow(other.mWindow)
{
    other.mWindow = nullptr;
}

VkSurfaceKHR
Window::createSurfaceForWindow(const Instance& instance) const {
    assert(mWindow != nullptr);

    VkSurfaceKHR surface;
    vkChecker(glfwCreateWindowSurface(instance.vkInstance(),
                                      mWindow,
                                      nullptr,
                                      &surface));

    return surface;
}

bool 
Window::shouldCloseWindow() const {
    assert(mWindow != nullptr);

    return glfwWindowShouldClose(mWindow) != 0;
}

void
Window::widthAndHeight(uint32_t& width, 
                       uint32_t& height) const {
    assert(mWindow != nullptr);
    int w;
    int h;
    glfwGetWindowSize(mWindow, &w, &h);
    width = static_cast<uint32_t>(w);
    height = static_cast<uint32_t>(h);
}

uint32_t
Window::width() const {
    assert(mWindow != nullptr);
    int w;
    int h;
    glfwGetWindowSize(mWindow, 
                      &w,
                      &h);
    return static_cast<uint32_t>(w);
}

uint32_t
Window::height() const {
    assert(mWindow != nullptr);
    int w;
    int h;
    glfwGetWindowSize(mWindow, 
                      &w, 
                      &h);
    return static_cast<uint32_t>(h);
}
}