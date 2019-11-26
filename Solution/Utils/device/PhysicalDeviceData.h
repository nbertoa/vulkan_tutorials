#ifndef UTILS_DEVICE_PHYSICAL_DEVICE_DATA
#define UTILS_DEVICE_PHYSICAL_DEVICE_DATA

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class Surface;
//
// Useful data for the PhysicalDevice which includes
// the vulkan device and queue family indices.
//
// The responsibility of this class is to check if the
// physical device supports the surface, device extensions,
// and the different queue families.
//
// This class is used internally by PhysicalDevice to 
// get information about all the candidate physical devices
// and choose the best fit.
//
class PhysicalDeviceData {
public:
    // * surface is used to check if the candidate physical device
    //   supports presentation and also if our swap chain is supported.
    //
    // * deviceExtensions contains the extensions that the device must support.
    //   Extensions may define new Vulkan commands, structures, and enumerants.
    //   Extensions may extend or change the behavior of the Vulkan API.
    PhysicalDeviceData(const VkPhysicalDevice physicalDevice,
                       const Surface& surface,
                       const std::vector<const char*>& deviceExtensions);

    VkPhysicalDevice
    vkPhysicalDevice() const;

    uint32_t
    graphicsSupportQueueFamilyIndex() const;

    uint32_t
    transferSupportQueueFamilyIndex() const;

    uint32_t
    presentationSupportQueueFamilyIndex() const;

    bool
    isSupported() const;

private:
    // A queue family just describes a set of queues with identical properties. 
    // In the following example you can see 3 VkQueueFamilyProperties with its information.
    // Queue number : 16
    // Queue flags : {Graphics | Compute | Transfer | SparseBinding}
    // Queue number : 1
    // Queue flags : {Transfer}
    // Queue number : 8
    // Queue flags : {Compute}
    // So in this example, the device supports three kinds of queues:
    // - One kind can do graphics, compute, transfer, and sparse binding operations, 
    //   and you can create up to 16 queues of that type.
    // - Another kind can only do transfer operations, and you can only create one queue 
    //   of this kind. Usually this is for asynchronously DMAing data between hostand device 
    //   memory on discrete GPUs, so transfers can be done concurrently with 
    //   independent graphics/compute operations.
    // - Finally, you can create up to 8 queues that are only capable of compute operations.
    //
    // Some queues might only correspond to separate queues in the host-side scheduler, 
    // other queues might correspond to actual independent queues in hardware. 
    // For example, many GPUs only have one hardware graphics queue, so even if you create 
    // two VkQueues from a graphics-capable queue family, command buffers submitted to those 
    // queues will progress through 
    // the kernel driver's command buffer scheduler independently, but will execute 
    // in some serial order on the GPU. 
    // But some GPUs have multiple compute-only hardware queues, so two VkQueues 
    // for a compute-only queue family might actually proceed independently and concurrently 
    // all the way through the GPU. Vulkan doesn't expose this.
    //
    // Bottom line, decide how many queues you can usefully use, based on how much 
    // concurrency you have. 
    // For many apps, a single "universal" queue is all they need. 
    // More advanced ones might have one graphics+compute queue, a separate compute-only queue 
    // for asynchronous compute work, and a transfer queue for async DMA. 
    // Then map what you'd like onto what's available; you may need to do your own multiplexing, 
    // e.g. on a device that doesn't have a compute-only queue family, you might create multiple 
    // graphics+compute queues instead, or serialize your async compute jobs onto your 
    // single graphics+compute queue yourself.
    //
    // * queueFamilyProperties:
    //
    //   - queueFlags is a bitmask of VkQueueFlagBits indicating 
    //     capabilities of the queues in this queue family (VK_QUEUE_): 
    //     . GRAPHICS_BIT, COMPUTE_BIT, TRANSFER_BIT, SPARSE_BINDING_BIT, PROTECTED_BIT.
    //   - queueCount is the unsigned integer count of queues in this queue family.
    //     Each queue family must support at least one queue.
    //   - timestampValidBits is the unsigned integer count of meaningful bits 
    //     in the timestamps written via vkCmdWriteTimestamp.
    //     The valid range for the count is 36..64 bits, or a value of 0, 
    //     indicating no support for timestamps.
    //     Bits outside the valid range are guaranteed to be zeros.
    //   - minImageTransferGranularity is the minimum granularity supported 
    //     for image transfer operations on the queues in this queue family.
    void
    queueFamilyProperties(std::vector<VkQueueFamilyProperties>& queueFamilyProperties);

    // * extensionProperties:
    //
    //   - extensionName is an array of VK_MAX_EXTENSION_NAME_SIZE char containing 
    //     a null-terminated UTF-8 string which is the name of the extension.
    //   - specVersion is the version of this extension.
    //     It is an integer, incremented with backward compatible changes.
    void
    extensionProperties(std::vector<VkExtensionProperties>& extensionProperties);

    bool
    isGraphicQueueFamilySupported();

    bool
    isTransferQueueFamilySupported();

    bool
    isPresentationSupported(const Surface& surface);

    bool
    areDeviceExtensionsSupported(const std::vector<const char*>& deviceExtensions);

    bool 
    isSwapChainSupported(const Surface& surface) const;

    bool
    areDeviceFeaturesSupported() const;
    
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    uint32_t mGraphicsSupportQueueFamilyIndex = 0;
    uint32_t mTransferSupportQueueFamilyIndex = 0;
    uint32_t mPresentationSupportQueueFamilyIndex = 0;
    bool mIsSupported = false;
};
}

#endif