#include "SystemInitializer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

#include "DebugUtils.h"
#include "Instance.h"
#include "Window.h"
#include "device/LogicalDevice.h"
#include "device/PhysicalDevice.h"
#include "resource/ImageSystem.h"
#include "shader/ShaderModuleSystem.h"

namespace vk {
namespace system_initializer {
void
initialize() {
    glfwChecker(glfwInit());

    Instance::initialize();

    Window::initialize(800,
                       600,
                       "Vulkan App");
    
    PhysicalDevice::initialize({VK_KHR_SWAPCHAIN_EXTENSION_NAME});

    LogicalDevice::initialize({VK_KHR_SWAPCHAIN_EXTENSION_NAME});
   
}

void
finalize() {
    ImageSystem::clear();

    ShaderModuleSystem::clear();

    LogicalDevice::finalize();

    PhysicalDevice::finalize();

    Window::finalize();

    Instance::finalize();

    glfwTerminate();
}
}
}