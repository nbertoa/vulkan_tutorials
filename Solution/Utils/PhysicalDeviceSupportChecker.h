#ifndef UTILS_PHYSICAL_DEVICE_SUPPORT_CHECKER
#define UTILS_PHYSICAL_DEVICE_SUPPORT_CHECKER

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class Instance;
class Surface;
// Class responsible to check if a VkPhysicalDevice is supported or not.
// If it is supported, it also stores the queue family indices.
class PhysicalDeviceData {
public:
    PhysicalDevice(const Instance& instance,
                   const Surface& surface);
    PhysicalDevice(PhysicalDevice&& other) noexcept;
    PhysicalDevice(const PhysicalDevice&) = delete;
    const PhysicalDevice& operator=(const PhysicalDevice&) = delete;

    VkPhysicalDevice
    vkPhysicalDevice() const;

    uint32_t
        graphicsSupportQueueFamilyIndex() const;

    uint32_t
        transferSupportQueueFamilyIndex() const;

    uint32_t
        presentationSupportQueueFamilyIndex() const;

    const std::vector<const char*>&
        deviceExtensions() const;

    // Returns the memory type index of this physical device that is
    // suitable. If there is not a suitable memory property, then it returns
    // std::numeric_limits<uint32_t>::max()
    //
    // memoryTypeFilter: will be used to specify the bit field of memory types
    // that are suitable.
    //
    // VkMemoryPropertyFlags:
    // - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit specifies that memory allocated with 
    //   this type is the most efficient for device access.
    //   This property will be set ifand only if the memory type belongs to a heap 
    //   with the VK_MEMORY_HEAP_DEVICE_LOCAL_BIT set.
    //
    // - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with 
    //   this type can be mapped for host access using vkMapMemory.
    //
    // - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache 
    //   management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
    //   are not needed to flush host writes to the device or make device writes visible 
    //   to the host, respectively.
    //
    // - VK_MEMORY_PROPERTY_HOST_CACHED_BIT bit specifies that memory allocated with this 
    //   type is cached on the host.Host memory accesses to uncached memory are slower than 
    //   to cached memory, however uncached memory is always host coherent.
    //
    // - VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit specifies that the memory type only 
    //   allows device access to the memory.Memory types must not have both 
    //   K_MEMORY_PROPERTY_LAZILY_ALLOCATED_BITand VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set.
    //   Additionally, the object’s backing memory may be provided by the implementation lazily 
    //   as specified in Lazily Allocated Memory.
    //
    // - VK_MEMORY_PROPERTY_PROTECTED_BIT bit specifies that the memory type only allows device 
    //   access to the memory, and allows protected queue operations to access the memory.
    //   Memory types must not have VK_MEMORY_PROPERTY_PROTECTED_BIT setand any of 
    //   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set, or VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, 
    //   or VK_MEMORY_PROPERTY_HOST_CACHED_BIT set.
    //
    // - VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD bit specifies that device accesses to 
    //   allocations of this memory type are automatically made availableand visible.
    //
    // - VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD bit specifies that memory allocated with 
    //   this type is not cached on the device.Uncached device memory is always device coherent.
    uint32_t
        memoryTypeIndex(const uint32_t memoryTypeFilter,
                        const VkMemoryPropertyFlags memoryPropertyFlags) const;

    bool
        isValidMemoryTypeIndex(const uint32_t memoryTypeIndex) const;

private:
    static void
        physicalDeviceQueueFamilyProperties(const VkPhysicalDevice physicalDevice,
                                            std::vector<VkQueueFamilyProperties>& queueFamilyProperties);

    static void
        physicalDeviceExtensionProperties(const VkPhysicalDevice physicalDevice,
                                          std::vector<VkExtensionProperties>& extensionProperties);

    static bool
        isGraphicQueueFamilySupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice,
                                                      uint32_t& queueFamilyIndex);

    static bool
        isTransferQueueFamilySupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice,
                                                       uint32_t& queueFamilyIndex);

    static bool
        isPresentationSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice,
                                                const Surface& surface,
                                                uint32_t& queueFamilyIndex);

    static bool
        areDeviceExtensionsSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice,
                                                     const std::vector<const char*>& deviceExtensions);

    static bool
        isSwapChainSupported(const VkPhysicalDevice physicalDevice,
                             const Surface& surface);

    struct PhysicalDeviceInfo {
        VkPhysicalDevice mDevice = VK_NULL_HANDLE;
        uint32_t mGraphicsSupportQueueFamilyIndex = 0;
        uint32_t mTransferSupportQueueFamilyIndex = 0;
        uint32_t mPresentationSupportQueueFamilyIndex = 0;
    };

    std::vector<PhysicalDeviceInfo>
        getCandidateDevices(const Instance& instance,
                            const Surface& surface) const;

    bool
        isPhysicalDeviceSuitable(const Surface& surface,
                                 PhysicalDeviceInfo& physicalDeviceInfo) const;

    void
        setPhysicalDevice(const std::vector<PhysicalDevice::PhysicalDeviceInfo>& candidateDevices);

    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    uint32_t mGraphicsSupportQueueFamilyIndex = 0;
    uint32_t mTransferSupportQueueFamilyIndex = 0;
    uint32_t mPresentationSupportQueueFamilyIndex = 0;

    std::vector<const char*> mDeviceExtensions;
};
}

#endif