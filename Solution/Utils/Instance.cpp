#include "Instance.h"

#include <cassert>

#include "DebugMessenger.h"
#include "DebugUtils.h"

namespace vk {
Instance::Instance() {
    createInstance();
}

Instance::~Instance() {
#ifndef NDEBUG // Debug
    delete mMessenger;
#endif

    vkDestroyInstance(mInstance, 
                      nullptr);
}

Instance::Instance(Instance&& other) noexcept
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

std::vector<PhysicalDeviceData>
Instance::getCandidatePhysicalDevices(const Surface& surface,
                                      const std::vector<const char*>& deviceExtensionNames) const {
    assert(mInstance != VK_NULL_HANDLE);

    const std::vector<VkPhysicalDevice> devices = physicalDevices();
    std::vector<PhysicalDeviceData> supportedPhysicalDevices;
    for (const VkPhysicalDevice& device : devices) {
        assert(device != VK_NULL_HANDLE);

        PhysicalDeviceData deviceData(device,
                                      surface,
                                      deviceExtensionNames);
        if (deviceData.isSupported()) {
            supportedPhysicalDevices.push_back(deviceData);
        }
    }

    return supportedPhysicalDevices;
}

std::vector<VkPhysicalDevice>
Instance::physicalDevices() const {
    assert(mInstance != nullptr);

    uint32_t physicalDeviceCount = 0;
    vkChecker(vkEnumeratePhysicalDevices(mInstance,
                                         &physicalDeviceCount,
                                         nullptr));
    assert(physicalDeviceCount > 0);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkChecker(vkEnumeratePhysicalDevices(mInstance,
                                         &physicalDeviceCount,
                                         physicalDevices.data()));

    return physicalDevices;
}

void
Instance::createInstance() {
    assert(mInstance == VK_NULL_HANDLE);
    assert(mMessenger == VK_NULL_HANDLE);

    const VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfoPtr = nullptr;
#ifndef NDEBUG // Debug
    const VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = DebugMessenger::messengerCreateInfo();
    debugMessengerCreateInfoPtr = &debugMessengerCreateInfo;
#endif

    // This is optional but it may provide some useful information to the driver to optimize 
    // for our specific application, for example, because it ises a well-known graphics 
    // engine with certain special behavior.
    //
    // VkApplicationInfo:
    // - pApplicationName
    // - applicationVersion contains the developer-supplied version number of the application.
    // - pEngineName (if any) used to create the application.
    // - engineVersion contains the developer-supplied 
    //   version number of the engine used to create the application.
    // - apiVersion must be the highest version of Vulkan that the application is designed 
    //   to use. The patch version number specified in apiVersion is ignored when creating 
    //   an instance object. Only the major and minor versions of the instance must 
    //   match those requested in apiVersion.
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = nullptr;
    appInfo.pEngineName = nullptr;
    appInfo.apiVersion = 0;

    // VkInstanceCreateInfo:
    // - pApplicationInfo is optional.
    //   If not nullptr, this information helps implementations recognize behavior inherent 
    //   to classes of applications. 
    // - enabledLayerCount
    // - ppEnabledLayerNames to enable for the created instance.
    // - enabledExtensionCount
    // - ppEnabledExtensionNames to enable.
    const std::vector<const char*> instanceExtensions = getInstanceExtensionNames();
    const std::vector<const char*> instanceLayers = getInstanceLayerNames();
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.flags = 0;    
    instanceInfo.pNext = debugMessengerCreateInfoPtr;
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    instanceInfo.ppEnabledExtensionNames = instanceExtensions.empty() ? nullptr : instanceExtensions.data();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
    instanceInfo.ppEnabledLayerNames = instanceLayers.empty() ? nullptr : instanceLayers.data();

    vkChecker(vkCreateInstance(&instanceInfo,
                               nullptr,
                               &mInstance));
    assert(mInstance != VK_NULL_HANDLE);

    // In debug mode, we need to create the debug messenger.
#ifndef NDEBUG // Debug
    mMessenger = new DebugMessenger(*this);
#endif
}

std::vector<const char*>
Instance::getInstanceLayerNames() {
    std::vector<const char*> instanceLayers;
#ifndef NDEBUG // Debug
    instanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");
    assert(areInstanceLayersSupported(instanceLayers));
#endif

    return instanceLayers;
}

bool
Instance::areInstanceLayersSupported(const std::vector<const char*>& requiredLayers) {
    uint32_t layerCount;
    vkChecker(vkEnumerateInstanceLayerProperties(&layerCount, 
                                                 nullptr));

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkChecker(vkEnumerateInstanceLayerProperties(&layerCount, 
                                                 availableLayers.data()));

    for (const char* layerName : requiredLayers) {
        assert(layerName != nullptr);
        bool layerFound = false;

        for (const VkLayerProperties& layerProperties : availableLayers) {
            if (strcmp(layerName, 
                       layerProperties.layerName) == 0) {
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
Instance::getInstanceExtensionNames() {
    // Vulkan is a platform agnostic API, which means that you need an extension
    // to interface with the window system.
    // GLFW has a handy built-in function that returns the extension(s)
    // it needs to do that which we can pass to the struct.
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    assert(glfwExtensions != nullptr);

    std::vector<const char*> instanceExtensions(glfwExtensions, 
                                                glfwExtensions + glfwExtensionCount);
#ifndef NDEBUG // Debug
    instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return instanceExtensions;
}
}