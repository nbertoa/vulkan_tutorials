#ifndef UTILS_DEVICE_LOGICAL_DEVICE
#define UTILS_DEVICE_LOGICAL_DEVICE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class PhysicalDevice;

//
// VkDevice wrapper.
//
// Vulkan separates the concept of physical and logical devices.
// A physical device usually represents a single complete implementation 
// of Vulkan (excluding instance-level functionality) available to the host, 
// of which there are a finite number.
// A logical device represents an instance of that implementation with its 
// own state and resources independent of other logical devices.
//
// VkDevice can be thought of as a logical device, or opened device.
// It represents logical connections to physical devices. 
// The created logical device is then the primary interface 
// to the physical device.
// It is the main object that represents an initialized Vulkan device that is ready to 
// create all other objects.
// This is a similar concept to the Device object in DirectX.
// During device creation, you need to specify which features you want to enable, 
// and some of them are fundamental like anisotropic texture filtering.
// You also must state all queues that will be in use, their number and their Queue Families.
// Each device exposes a number of queue families each having one or more queues. 
// All queues in a queue family support the same operations.
//
// You need the LogicalDevice to:
// - Get the queue that you want to use.
//
// To create/use the LogicalDevice you need:
// - PhysicalDevice
//
class LogicalDevice {
public:
    LogicalDevice() = delete;
    ~LogicalDevice() = delete;
    LogicalDevice(LogicalDevice&& other) = delete;
    LogicalDevice(const LogicalDevice&) = delete;
    const LogicalDevice& operator=(const LogicalDevice&) = delete;

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

    static VkDevice
    vkDevice();

    // Creating a logical device also creates the queues associated with that device.
    //
    // Queue is an object representing a queue of commands to be executed on the device.
    // All the actual work to be done by the GPU is requested by filling CommandBuffers
    // and submitting them to Queues, using the function vkQueueSubmit.
    //
    // If you have multiple queues like the main graphics queue and a compute queue, 
    // you can submit different CommandBuffers to each of them.
    // This way you can enable asynchronous compute, which can lead to a 
    // substantial speed up if done right.

    static VkQueue
    graphicsQueue();

    static VkQueue
    transferQueue();

    static VkQueue
    presentationQueue();   

private:
    static VkDevice
    createLogicalDevice(const std::vector<const char*>& deviceExtensionNames);

    // Get the necessary structures to create the queues.
    // 
    // VkDeviceQueueCreateInfo:
    // - flags bitmask indicating behavior of the queue.
    // - queueFamilyIndex to create on this device. This index corresponds to 
    //   the index of an element of the pQueueFamilyProperties array that was returned 
    //   by vkGetPhysicalDeviceQueueFamilyProperties.
    // - queueCount to create in the queue family indicated by queueFamilyIndex.
    // - pQueuePriorities array that contains normalized floating point values, 
    //   specifying priorities of work that will be submitted to each created queue.
    static std::vector<VkDeviceQueueCreateInfo> 
    physicalDeviceQueuesCreateInfo();
   
    // - physicalDevice is used to get the queue family indices
    //   used to create the queues
    static void
    initQueues();

    static VkDevice mLogicalDevice;

    static VkQueue mGraphicsQueue;
    static VkQueue mTransferQueue;
    static VkQueue mPresentationQueue;
};
}

#endif