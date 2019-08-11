#include "utils/AppInstance.h"
#include "utils/LogicalDevice.h"
#include "utils/SwapChain.h"
#include "utils/Window.h"
#include "utils/WindowSurface.h"

using namespace vk;

int main() {
    const uint32_t windowWidth = 800;
    const uint32_t windowHeight = 600;

    Window window(windowWidth, 
                  windowHeight, 
                  "Vulkan window");

    AppInstance instance;

    WindowSurface windowSurface(instance.vkInstance(), 
                                window.glfwWindow());

    LogicalDevice logicalDevice(instance.vkInstance(), 
                                windowSurface);

    SwapChain swapChain(windowWidth, 
                        windowHeight, 
                        windowSurface, 
                        logicalDevice);

    while (glfwWindowShouldClose(&window.glfwWindow()) == 0) {
        glfwPollEvents();
    }
}