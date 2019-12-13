#ifndef UTILS_INSTANCE
#define UTILS_INSTANCE

#include <vector>
#include <vulkan/vulkan.hpp>

#include "device/PhysicalDeviceData.h"

namespace vulkan {
#ifdef _DEBUG
class DebugMessenger;
#endif

//
// Instance wrapper.
//
// Instance is the first object you create.
// In our case, we have a single and global Instance.
// 
// There is no global state in Vulkan and all per-application 
// state is stored in a vk::Instance object. 
//
// Creating a Instance object initializes the Vulkan library 
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

    static const vk::Instance&
    instance();

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
        
    static bool 
    areInstanceLayersSupported(const std::vector<const char*>& instanceLayers);
    
    static vk::Instance mInstance;

#ifdef _DEBUG
    static DebugMessenger* mMessenger;
#endif    
};
}

#endif