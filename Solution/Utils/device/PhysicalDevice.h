#ifndef UTILS_DEVICE_PHYSICAL_DEVICE
#define UTILS_DEVICE_PHYSICAL_DEVICE

#include <vector>
#include <vulkan/vulkan.hpp>

#include "PhysicalDeviceData.h"

namespace vk2 {
//
// PhysicalDevice wrapper.
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
// A Vulkan application will first query for all physical devices in a system.   
// 
// You need the PhysicalDevice to:
// - Get the memory heap for DeviceMemory creation:
//   Device memory is explicitly managed by the application.
//   
//   Each device may advertise one or more heaps, representing different areas of memory.
//   
//   Memory heaps are either device local or host local, but are always visible to the device.
//   
//   On other architectures, there may only be a single heap that can be used for any purpose.
//
// - Get queue family index which are needed for Queues creation in LogicalDevice:  
//   Queue is an object representing a queue of commands to be executed on the device.
//
//   The set of queues supported by a device is partitioned into families 
//   (idenfitied by its index).
//
//   Each family supports one or more types of functionality and 
//   may contain multiple queues with similar characteristics.
//
//   Queues within a single family are considered compatible with one another, 
//   and work produced for a family of queues can be executed on any queue within that family.
//
//   Specification defines four types of functionality that queues may support: 
//   * graphics, compute, transfer, and sparse memory management.
//
// To create/use the PhysicalDevice you need:
// - Instance
//
class PhysicalDevice {
public:
    static void
    initialize(const std::vector<const char*>& deviceExtensionNames);

    static void
    finalize();

    static VkPhysicalDevice 
    device();

    static uint32_t
    graphicsQueueFamilyIndex();

    static uint32_t
    transferQueueFamilyIndex();

    static uint32_t
    presentationQueueFamilyIndex();

    // Returns the memory type index of this physical device that is
    // suitable. If there is not a suitable memory property, then it returns
    // std::numeric_limits<uint32_t>::max()
    //
    // * memoryTypeFilter is to specify the bit field of memory types
    //   that are suitable. It holds specific requirements for 
    //   the memory blob like visible to the host, coherent (between CPU and GPU) and cached.
    //   There may be an arbitrary combination of these, depending on the device driver.
    // 
    // * memoryPropertyFlags is used to check if there is a memory type in the physical device
    //   that is suitable for the buffer that also 
    //   has all of the properties we need (VK_MEMORY_PROPERTY_):
    //
    //   - DEVICE_LOCAL_BIT, HOST_VISIBLE_BIT, HOST_COHERENT_BIT, HOST_CACHED_BIT,
    //     LAZILY_ALLOCATED_BIT, PROTECTED_BIT, HOST_VISIBLE_BIT, DEVICE_COHERENT_BIT_AMD,
    //     DEVICE_UNCACHED_BIT_AMD
    static uint32_t
    memoryTypeIndex(const uint32_t memoryTypeFilter,
                    const VkMemoryPropertyFlags memoryPropertyFlags);

    static bool
    isValidMemoryTypeIndex(const uint32_t memoryTypeIndex);

private:           
    PhysicalDevice() = delete;
    PhysicalDevice(PhysicalDevice&& other) = delete;
    PhysicalDevice(const PhysicalDevice&) = delete;
    const PhysicalDevice& operator=(const PhysicalDevice&) = delete;

    static vk::PhysicalDevice mPhysicalDevice;

    static uint32_t mGraphicsQueueFamilyIndex;
    static uint32_t mTransferQueueFamilyIndex;
    static uint32_t mPresentationQueueFamilyIndex;

    static std::vector<const char*> mDeviceExtensions;
};
}

#endif