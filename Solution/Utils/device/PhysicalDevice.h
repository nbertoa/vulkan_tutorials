#ifndef UTILS_DEVICE_PHYSICAL_DEVICE
#define UTILS_DEVICE_PHYSICAL_DEVICE

#include <vector>
#include <vulkan/vulkan.h>

#include "PhysicalDeviceData.h"

namespace vk {
//
// VkPhysicalDevice wrapper.
//
// Vulkan separates the concept of physical and logical devices.
// A physical device usually represents a single complete implementation 
// of Vulkan (excluding instance-level functionality) available to the host, 
// of which there are a finite number.
// A logical device represents an instance of that implementation with its 
// own state and resources independent of other logical devices.
//
// PhysicalDevice represents a specific Vulkan-compatible device, like a graphics card. 
//
// PhysicalDevice can enumerate all available types of Queue Families. 
// The graphics queue is the main one, but you may also have additional ones 
// that support only Compute or Transfer.
//
// PhysicalDevice can also enumerate Memory Heaps and Memory Types inside them. 
// A Memory Heap represents a specific pool of RAM. 
// It may abstract your system RAM on the motherboard or a certain memory space in video RAM 
// on a dedicated graphics card, or any other host- or device-specific memory the implementation 
// wants to expose. 
// You must specify the Memory Type when allocating memory. 
// It holds specific requirements for the memory blob like visible to the host, 
// coherent (between CPU and GPU) and cached. 
// There may be an arbitrary combination of these, depending on the device driver.
//
// You enumerate these from Instance and you can then query them for their vendorID, 
// deviceID, and supported features, as well as other properties and limits.
//
// You need the PhysicalDevice to:
// - Get the memory heap from where you can get the memory type (indices uint32_t)
//   which is needed for DeviceMemory creation.
// - Get queue family (indices uint32_t) which are needed for Queues creation 
//   in LogicalDevice. (read below)
//
// To create/use the PhysicalDevice you need:
// - Instance
//
// To understand queue families, you first have to understand queues.
// A queue is something you submit command buffers to, and command buffers submitted 
// to a queue are executed in order[*1] relative to each other.
// Command buffers submitted to different queues are unordered relative to each other unless 
// you explicitly synchronize them with VkSemaphore. You can only submit work to a queue 
// from one thread at a time, but different threads can submit work to different 
// queues simultaneously.
//
// Each queue can only perform certain kinds of operations:
// - Graphics queues can run graphics pipelines started by vkCmdDraw* commands.
// - Compute queues can run compute pipelines started by vkCmdDispatch* .
// - Transfer queues can perform transfer(copy) operations from vkCmdCopy*.
// - Sparse binding queues can change the binding of sparse resources to memory with 
//   vkQueueBindSparse
// (note this is an operation submitted directly to a queue, not a command in a command buffer).
//
// Some queues can perform multiple kinds of operations. Every command that can be submitted 
// to a queue have a "Command Properties" table that lists what queue types can execute the command.
// A queue family just describes a set of queues with identical properties.
//
// [*1] Oversimplifying a bit. They start in order, but are allowed to proceed independently 
// after that and complete out of order. 
// Independent progress of different queues is not guaranteed though.
//
class PhysicalDevice {
public:
    PhysicalDevice() = delete;
    PhysicalDevice(PhysicalDevice&& other) = delete;
    PhysicalDevice(const PhysicalDevice&) = delete;
    const PhysicalDevice& operator=(const PhysicalDevice&) = delete;

    static void
    initialize(const std::vector<const char*>& deviceExtensionNames);

    static void
    finalize();

    static VkPhysicalDevice 
    vkPhysicalDevice();

    static uint32_t
    graphicsSupportQueueFamilyIndex();

    static uint32_t
    transferSupportQueueFamilyIndex();

    static uint32_t
    presentationSupportQueueFamilyIndex();

    // Returns the memory type index of this physical device that is
    // suitable. If there is not a suitable memory property, then it returns
    // std::numeric_limits<uint32_t>::max()
    //
    // PhysicalDevice can also enumerate Memory Heaps and Memory Types inside them. 
    // A Memory Heap represents a specific pool of RAM. 
    // It may abstract your system RAM on the motherboard or a certain memory space in video RAM 
    // on a dedicated graphics card, or any other host- or device-specific memory 
    // the implementation wants to expose. 
    // You must specify the Memory Type when allocating memory. 
    // It holds specific requirements for the memory blob like visible to the host, 
    // coherent (between CPU and GPU) and cached. 
    // There may be an arbitrary combination of these, depending on the device driver.
    //
    // * memoryTypeFilter is to specify the bit field of memory types
    //   that are suitable.
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
    static VkPhysicalDevice mPhysicalDevice;

    static uint32_t mGraphicsSupportQueueFamilyIndex;
    static uint32_t mTransferSupportQueueFamilyIndex;
    static uint32_t mPresentationSupportQueueFamilyIndex;

    static std::vector<const char*> mDeviceExtensions;
};
}

#endif