#include "AppInstance.h"

#include <cassert>

#include "DebugMessenger.h"
#include "DebugUtils.h"

namespace vk {
Instance::AppInstance() {
    createInstance();
}

Instance::~AppInstance() {
#ifndef NDEBUG // Debug
    delete mMessenger;
#endif

    vkDestroyInstance(mInstance, nullptr);
}

Instance::AppInstance(AppInstance&& other) noexcept
    : mInstance(other.mInstance)
#ifndef NDEBUG // Debug
    , mMessenger(other.mMessenger)
#endif
{
    other.mInstance = VK_NULL_HANDLE;

#ifndef NDEBUG // Debug
    other.mMessenger = nullptr;
#endif    
}

const VkInstance& 
Instance::vkInstance() const {
    assert(mInstance != nullptr);
    return mInstance;
}

void
AppInstance::createInstance() {
    assert(mInstance == VK_NULL_HANDLE);
    assert(mMessenger == VK_NULL_HANDLE);

    const VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfoPtr = nullptr;
#ifndef NDEBUG // Debug
    const VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = messengerCreateInfo();
    debugMessengerCreateInfoPtr = &debugMessengerCreateInfo;
#endif

    // This is optional but it may provide some useful information to the driver to optimize 
    // for our specific application, for example, because it ises a well-known graphics 
    // engine with certain special behavior,
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    // Point to extension information in the future.
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = nullptr;
    appInfo.pEngineName = nullptr;
    appInfo.apiVersion = 0;

    // The instance is the connection between our application and the Vulkan library.
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.flags = 0;    
    instanceInfo.pNext = debugMessengerCreateInfoPtr;

    // Specify the desired global extensions
    const std::vector<const char*> instanceExtensions = getRequiredInstanceExtensions();
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    instanceInfo.ppEnabledExtensionNames = instanceExtensions.empty() ? nullptr : instanceExtensions.data();

    // Get instance layers
    const std::vector<const char*> instanceLayers = getInstanceLayers();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
    instanceInfo.ppEnabledLayerNames = instanceLayers.empty() ? nullptr : instanceLayers.data();

    vkChecker(vkCreateInstance(&instanceInfo,
                               nullptr,
                               &mInstance));
    assert(mInstance != VK_NULL_HANDLE);

    // In debug mode, we need to create the debug messenger.
#ifndef NDEBUG // Debug
    mMessenger = new DebugMessenger(*this, *debugMessengerCreateInfoPtr);
#endif
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
    // Vulkan is a platform agnostic API, which means that you need an extension
    // to interface with the window system.
    // GLFW has a handy built-in function that returns the extension(s)
    // it needs to do that which we can pass to the struct.
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
    // Specify all the types of severities you would like 
    // your callback to be called for.
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    // Filter which types of messages your callback
    // is notified about.
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    // Pointer to the callback function. 
    createInfo.pfnUserCallback = DebugMessenger::debugCallback;

    return createInfo;
}
#endif
}