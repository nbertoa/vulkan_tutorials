#include "AppInstance.h"

#include "DebugMessenger.h"
#include "DebugUtils.h"

namespace vk {
AppInstance::AppInstance() {
    const VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfoPtr = nullptr;
#ifndef NDEBUG // Debug
    const VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = messengerCreateInfo();
    debugMessengerCreateInfoPtr = &debugMessengerCreateInfo;
#endif

    createInstance(debugMessengerCreateInfoPtr);

    // In debug mode, we need to create the debug messenger.
#ifndef NDEBUG // Debug
    mMessenger = new DebugMessenger(mInstance, *debugMessengerCreateInfoPtr);
#endif
}

AppInstance::~AppInstance() {
    assert(mInstance != VK_NULL_HANDLE);

#ifndef NDEBUG // Debug
    assert(mMessenger != nullptr);
    delete mMessenger;
#endif

    vkDestroyInstance(mInstance, nullptr);
}

void
AppInstance::createInstance(const VkDebugUtilsMessengerCreateInfoEXT* debugMessengerInfo) {
    assert(mInstance == VK_NULL_HANDLE);

    // This is optional but it may provide some useful information to the driver to optimize 
    // for our specific application, for example, because it ises a well-known graphics 
    // engine with certain special behavior,
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = nullptr;
    appInfo.pEngineName = nullptr;
    appInfo.apiVersion = 0;

    // The instance is the connection between our application and the Vulkan library.
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.pNext = debugMessengerInfo;

    // Set instance extensions
    const std::vector<const char*> instanceExtensions = getRequiredInstanceExtensions();
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    instanceInfo.ppEnabledExtensionNames = instanceExtensions.empty() ? nullptr : instanceExtensions.data();

    // Get instance layers
    const std::vector<const char*> instanceLayers = getInstanceLayers();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
    instanceInfo.ppEnabledLayerNames = instanceLayers.empty() ? nullptr : instanceLayers.data();

    vkChecker(vkCreateInstance(&instanceInfo, nullptr, &mInstance));
    assert(mInstance != VK_NULL_HANDLE);
}

std::vector<const char*>
AppInstance::getInstanceLayers() {
    std::vector<const char*> instanceLayers;
#ifndef NDEBUG // Debug
    instanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");
    assert(areInstanceLayersSupported(instanceLayers));
#endif

    return instanceLayers;
}

bool
AppInstance::areInstanceLayersSupported(const std::vector<const char*>& requiredLayers) {
    uint32_t layerCount;
    vkChecker(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkChecker(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

    for (const char* layerName : requiredLayers) {
        assert(layerName != nullptr);
        bool layerFound = false;

        for (const VkLayerProperties& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (layerFound == false) {
            return false;
        }
    }

    return true;
}

std::vector<const char*>
AppInstance::getRequiredInstanceExtensions() {
    // Get extensions required by GLFW to be able to Vulkan surfaces for GLFW windows.
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    assert(glfwExtensions != nullptr);

    std::vector<const char*> instanceExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
#ifndef NDEBUG // Debug
    instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return instanceExtensions;
}

#ifndef NDEBUG
VkDebugUtilsMessengerCreateInfoEXT
AppInstance::messengerCreateInfo() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    createInfo.pfnUserCallback = DebugMessenger::debugCallback;
    createInfo.pUserData = nullptr;

    return createInfo;
}
#endif
}