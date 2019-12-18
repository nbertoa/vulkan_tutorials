#include "Instance.h"

#include <cassert>

#include "DebugMessenger.h"

namespace vulkan {
vk::Instance 
Instance::mInstance;

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

    vk::InstanceCreateInfo info;
    info.setEnabledExtensionCount(static_cast<uint32_t>(instanceExtensionNames.size()));
    info.setPpEnabledExtensionNames(instanceExtensionNames.empty() ? nullptr : instanceExtensionNames.data());
    info.setEnabledLayerCount(static_cast<uint32_t>(instanceLayerNames.size()));
    info.setPpEnabledLayerNames(instanceLayerNames.empty() ? nullptr : instanceLayerNames.data());
    mInstance = vk::createInstance(info);

#ifdef _DEBUG
    mMessenger = new DebugMessenger();
#endif
}

void
Instance::finalize() {
#ifdef _DEBUG
    delete mMessenger;
#endif

    mInstance.destroy();
}

const vk::Instance& 
Instance::instance() {
    assert(mInstance != VK_NULL_HANDLE);
    return mInstance;
}

std::vector<PhysicalDeviceData>
Instance::getCandidatePhysicalDevices(const std::vector<const char*>& deviceExtensionNames) {
    assert(mInstance != VK_NULL_HANDLE);

    std::vector<PhysicalDeviceData> supportedPhysicalDevices;
    for (const vk::PhysicalDevice device : mInstance.enumeratePhysicalDevices()) {
        assert(device != VK_NULL_HANDLE);

        PhysicalDeviceData deviceData(device,
                                      deviceExtensionNames);
        if (deviceData.isSupported()) {
            supportedPhysicalDevices.emplace_back(deviceData);
        }
    }

    return supportedPhysicalDevices;
}

bool
Instance::areInstanceLayersSupported(const std::vector<const char*>& requiredLayers) {
    const std::vector<vk::LayerProperties> availableLayers =
        vk::enumerateInstanceLayerProperties();

    for (const char* layerName : requiredLayers) {
        assert(layerName != nullptr);
        bool layerFound = false;

        for (const vk::LayerProperties& layerProperties : availableLayers) {
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