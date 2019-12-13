#include "Window.h"

#include "DebugUtils.h"
#include "Instance.h"

namespace vulkan {
GLFWwindow* 
Window::mWindow = nullptr;

vk::SurfaceKHR 
Window::mSurface;

void
Window::initialize(const uint32_t width, 
                   const uint32_t height,
                   const char* title) {
    assert(width > 0);
    assert(height > 0);
    assert(title != nullptr);
    assert(mWindow == nullptr);
    assert(mSurface == VK_NULL_HANDLE);
    
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

    vkChecker(glfwCreateWindowSurface(Instance::instance(),
                                      mWindow,
                                      nullptr,
                                      reinterpret_cast<VkSurfaceKHR*>(&mSurface)));
}

void
Window::finalize() {
    assert(mSurface != VK_NULL_HANDLE);
    Instance::instance().destroySurfaceKHR(mSurface);

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

vk::SurfaceKHR
Window::surface() {
    assert(mSurface != VK_NULL_HANDLE);
    return mSurface;
}

bool 
Window::shouldCloseWindow() {
    assert(mWindow != nullptr);

    return glfwWindowShouldClose(mWindow) != 0;
}

void
Window::widthAndHeight(uint32_t& width, 
                       uint32_t& height) {
    assert(mWindow != nullptr);
    int w;
    int h;
    glfwGetWindowSize(mWindow, 
                      &w, 
                      &h);
    width = static_cast<uint32_t>(w);
    height = static_cast<uint32_t>(h);
}

uint32_t
Window::width() {
    assert(mWindow != nullptr);
    int w;
    int h;
    glfwGetWindowSize(mWindow, 
                      &w,
                      &h);
    return static_cast<uint32_t>(w);
}

uint32_t
Window::height() {
    assert(mWindow != nullptr);
    int w;
    int h;
    glfwGetWindowSize(mWindow, 
                      &w, 
                      &h);
    return static_cast<uint32_t>(h);
}
}