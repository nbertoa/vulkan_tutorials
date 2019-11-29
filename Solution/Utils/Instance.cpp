#include "Instance.h"

#include <cassert>

#include "DebugMessenger.h"
#include "DebugUtils.h"

namespace vk2 {
VkInstance 
Instance::mInstance = VK_NULL_HANDLE;

#ifdef _DEBUG
DebugMessenger* 
Instance::mMessenger = nullptr;
#endif  

void
Instance::initialize(const std::vector<const char*>& instanceExtensionNames,
                     const std::vector<const char*>& instanceLayerNames) {
    assert(mInstance == VK_NULL_HANDLE);
#ifdef _DEBUG
    assert(mMessenger == VK_NULL_HANDLE);
#endif

    assert(areInstanceLayersSupported(instanceLayerNames));

    const VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfoPtr = nullptr;
#ifdef _DEBUG
    const VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo =
        DebugMessenger::messengerCreateInfo();
    debugMessengerCreateInfoPtr = &debugMessengerCreateInfo;
#endif

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = debugMessengerCreateInfoPtr;
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionNames.size());
    instanceInfo.ppEnabledExtensionNames = 
        instanceExtensionNames.empty() ? nullptr : instanceExtensionNames.data();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayerNames.size());
    instanceInfo.ppEnabledLayerNames = 
        instanceLayerNames.empty() ? nullptr : instanceLayerNames.data();

    vkChecker(vkCreateInstance(&instanceInfo,
                               nullptr,
                               &mInstance));

#ifdef _DEBUG
    mMessenger = new DebugMessenger();
#endif
}

void
Instance::finalize() {
#ifdef _DEBUG
    delete mMessenger;
#endif

    vkDestroyInstance(mInstance, 
                      nullptr);
}

const VkInstance& 
Instance::vkInstance() {
    assert(mInstance != nullptr);
    return mInstance;
}

std::vector<PhysicalDeviceData>
Instance::getCandidatePhysicalDevices(const std::vector<const char*>& deviceExtensionNames) {
    assert(mInstance != VK_NULL_HANDLE);

    const std::vector<VkPhysicalDevice> devices = physicalDevices();
    std::vector<PhysicalDeviceData> supportedPhysicalDevices;
    for (const VkPhysicalDevice& device : devices) {
        assert(device != VK_NULL_HANDLE);

        PhysicalDeviceData deviceData(device,
                                      deviceExtensionNames);
        if (deviceData.isSupported()) {
            supportedPhysicalDevices.emplace_back(deviceData);
        }
    }

    return supportedPhysicalDevices;
}

std::vector<VkPhysicalDevice>
Instance::physicalDevices() {
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
}