#ifndef UTILS_INSTANCE
#define UTILS_INSTANCE

#include <vector>
#include <vulkan/vulkan.h>

#include "device/PhysicalDeviceData.h"

namespace vk2 {
#ifdef _DEBUG
class DebugMessenger;
#endif

//
// VkInstance wrapper.
//
// Instance is the first object you create.
// In our case, we have a single and global Instance.
// 
// There is no global state in Vulkan and all per-application 
// state is stored in a VkInstance object. 
//
// Creating a VkInstance object initializes the Vulkan library 
// and allows the application to pass information about itself 
// to the implementation.
//
// You must specify all layers (like the Validation Layer) and 
// all extensions you want to enable when creating an Instance.
//
// You need the Instance to:
// - Create the Window's surface.
// - Get the PhysicalDevices
//
class Instance {
public:
    // Preconditions:
    // - GLFW must be initialized first.
    //
    // You must specify all layers (like the Validation Layer) and 
    // all extensions you want to enable when creating an Instance. 
    static void
    initialize(const std::vector<const char*>& instanceExtensionNames,
               const std::vector<const char*>& instanceLayerNames);

    static void
    finalize();

    static const VkInstance&
    vkInstance();

    // Return a list of candidate physical devices
    // based on window's surface support and device extension support.
    static std::vector<PhysicalDeviceData>
    getCandidatePhysicalDevices(const std::vector<const char*>& deviceExtensionNames);

private:
    Instance() = delete;
    ~Instance() = delete;
    Instance(Instance&& other) = delete;
    Instance(const Instance&) = delete;
    const Instance& operator=(const Instance&) = delete;

    static std::vector<VkPhysicalDevice>
    physicalDevices();
    
    static bool 
    areInstanceLayersSupported(const std::vector<const char*>& instanceLayers);
    
    static VkInstance mInstance;

#ifdef _DEBUG
    static DebugMessenger* mMessenger;
#endif    
};
}

#endif