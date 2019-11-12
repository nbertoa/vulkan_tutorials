#ifndef UTILS_INSTANCE
#define UTILS_INSTANCE

#include <vector>
#include <vulkan/vulkan.h>

#include "device/PhysicalDeviceData.h"

namespace vk {
#ifdef _DEBUG
class DebugMessenger;
#endif
class Surface;

//
// VkInstance wrapper.
//
// Instance is the first object you create.
//
// There is no global state in Vulkan and all per-application 
// state is stored in a VkInstance object. 
//
// It represents the connection from your application to the Vulkan runtime
// and therefore only should exist once in your application.
//
// Creating a VkInstance object initializes the Vulkan library 
// and allows the application to pass information about itself 
// to the implementation.
//
// It also stores all application specific state required to use Vulkan.
//
// Therefore you must specify all layers (like the Validation Layer) and 
// all extensions you want to enable when creating an Instance.
//
// You need the Instance to:
// - Create the Surface.
// - Get the PhysicalDevices
//
// Assumptions:
// - Instance layers and extensions are specified internally (check .cpp file)
//
class Instance {
public:
    // Preconditions:
    // - GLFW must be initialized first.
    Instance();
    ~Instance();
    Instance(Instance&& other) noexcept;
    Instance(const Instance&) = delete;
    const Instance& operator=(const Instance&) = delete;

    const VkInstance& 
    vkInstance() const;

    // Return a list of candidate physical devices
    // based on surface support and device extension support.
    std::vector<PhysicalDeviceData>
    getCandidatePhysicalDevices(const Surface& surface,
                                const std::vector<const char*>& deviceExtensionNames) const;

private:
    std::vector<VkPhysicalDevice>
    physicalDevices() const;

    void 
    createInstance();

    // Validation layers are optional components that hook into Vulkan function calls
    // to apply additional operations. We use them for debugging functionality.
    static std::vector<const char*> 
    getInstanceLayerNames();

    static bool 
    areInstanceLayersSupported(const std::vector<const char*>& instanceLayers);

    // Vulkan is a platform agnostic API, which means that we need an extension
    // to interface with different systems, like for example, the window system.
    static std::vector<const char*> 
    getInstanceExtensionNames();
    
    VkInstance mInstance = VK_NULL_HANDLE;

#ifdef _DEBUG
    DebugMessenger* mMessenger = nullptr;
#endif    
};
}

#endif