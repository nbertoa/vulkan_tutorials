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

namespace {
std::vector<const char*>
getInstanceLayerNames() {
    std::vector<const char*> instanceLayers;
#ifdef _DEBUG
    // Validation: the main, comprehensive Khronos validation layer.
    // This layer encompasses the entire functionality of the deprecated layers, 
    // and supercedes them. 
    // As the other layers are deprecated this layer should be used for all validation 
    // going forward.
    instanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");

    // Utility: print API calls and their parameters and values
    //instanceLayers.emplace_back("VK_LAYER_LUNARG_api_dump");

    // Utility: allows modification of an actual device's reported features, limits, 
    // and capabilities
    //instanceLayers.emplace_back("VK_LAYER_LUNARG_device_simulation");

    // Utility: outputs specified frames to an image file as they are presented
    //instanceLayers.emplace_back("VK_LAYER_LUNARG_screenshot");
#endif

    // Utility: outputs the frames-per-second of the target application in 
    // the applications title bar
    instanceLayers.emplace_back("VK_LAYER_LUNARG_monitor");

    return instanceLayers;
}

std::vector<const char*>
getInstanceExtensionNames() {
    // Vulkan is a platform agnostic API, which means that you need an extension
    // to interface with the window system.
    // GLFW has a handy built-in function that returns the extension(s)
    // it needs to do that which we can pass to the struct.
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    assert(glfwExtensions != nullptr);

    std::vector<const char*> instanceExtensions(glfwExtensions,
                                                glfwExtensions + glfwExtensionCount);
#ifdef _DEBUG
    instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return instanceExtensions;
}
}

namespace vulkan {
namespace system_initializer {
void
initialize() {
    glfwChecker(glfwInit());

    Instance::initialize(getInstanceExtensionNames(),
                         getInstanceLayerNames());

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