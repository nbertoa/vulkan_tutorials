#ifndef UTILS_DEVICE_LOGICAL_DEVICE
#define UTILS_DEVICE_LOGICAL_DEVICE

#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

namespace vk2 {
class PhysicalDevice;

//
// VkDevice wrapper.
//
// Once Vulkan is initialized, devices and queues are the 
// primary objects used to interact with a Vulkan implementation.
//
// Vulkan separates the concept of physical and logical devices.
//
// A physical device usually represents a single complete implementation 
// of Vulkan (excluding instance-level functionality) available to the host, 
// of which there are a finite number.
//
// A logical device represents an instance of that implementation with its 
// own state and resources independent of other logical devices.
//
// You need the LogicalDevice to:
// - Creation of queues.
//   When retrieving a handle to the queue via vkGetDeviceQueue, 
//   the queue family index is used to select which queue family to retrieve the VkQueue 
//
// - Creation and tracking of various synchronization constructs.
//
// - Allocating, freeing, and managing memory.
//
// - Creation and destruction of command buffers and command buffer pools.
//
// - Creation, destruction, and management of graphics  state.
//
// To create/use the LogicalDevice you need:
// - PhysicalDevice
//
class LogicalDevice {
public:
    // The global physical device will be used to get the 
    // queue families (indices uint32_t) which are needed for VKQueues creation.
    //
    // Extensions may define new Vulkan commands, structures, and enumerants.
    // Extensions may extend or change the behavior of the Vulkan API.
    //
    // The global physical device is used to create this logical device.
    static void
    initialize(const std::vector<const char*>& deviceExtensionNames);

    static void
    finalize();

    static vk::Device
    device();
    
    static vk::Queue
    graphicsQueue();

    static vk::Queue
    transferQueue();

    static vk::Queue
    presentationQueue();   

private:
    LogicalDevice() = delete;
    ~LogicalDevice() = delete;
    LogicalDevice(LogicalDevice&& other) = delete;
    LogicalDevice(const LogicalDevice&) = delete;
    const LogicalDevice& operator=(const LogicalDevice&) = delete;

    static void
    initLogicalDevice(const std::vector<const char*>& deviceExtensionNames);

    // Get the necessary structures to create the queues
    static std::vector<vk::DeviceQueueCreateInfo> 
    physicalDeviceQueuesCreateInfo(const float& queuePriority);
   
    // - physicalDevice is used to get the queue family indices
    //   used to create the queues
    static void
    initQueues();

    static vk::Device mLogicalDevice;

    static vk::Queue mGraphicsQueue;
    static vk::Queue mTransferQueue;
    static vk::Queue mPresentationQueue;
};
}

#endif