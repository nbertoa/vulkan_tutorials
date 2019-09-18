#ifndef UTILS_PHYSICAL_DEVICE
#define UTILS_PHYSICAL_DEVICE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class AppInstance;
class Surface;

// VkPhysicalDevice wrapper.
// It represents a physical device installed in the system.
//
// Vulkan separates the concept of physical and logical devices.
// A physical device usually represents a single complete implementation 
// of Vulkan(excluding instance - level functionality) available to the host, 
// of which there are a finite number.
// A logical device represents an instance of that implementation with its 
// own stateand resources independent of other logical devices.
class PhysicalDevice {
public:
    PhysicalDevice(const AppInstance& appInstance, 
                   const Surface& surface);
    PhysicalDevice(PhysicalDevice&& other) noexcept;
    PhysicalDevice(const PhysicalDevice&) = delete;
    const PhysicalDevice& operator=(const PhysicalDevice&) = delete;

    VkPhysicalDevice vkPhysicalDevice() const;

    uint32_t graphicsSupportQueueFamilyIndex() const;
    uint32_t transferSupportQueueFamilyIndex() const;
    uint32_t presentationSupportQueueFamilyIndex() const;

    const std::vector<const char*>& deviceExtensions() const;

    // memoryTypeFilter will be used to specify the bit field of memory types
    // that are suitable.
    // Returns the memory type index of this physical device that is
    // suitable. If there is not a suitable memory property, then it returns
    // std::numeric_limits<uint32_t>::max()
    uint32_t memoryTypeIndex(const uint32_t memoryTypeFilter,
                             const VkMemoryPropertyFlags memoryPropertyFlags) const;

    bool isValidMemoryTypeIndex(const uint32_t memoryTypeIndex) const;

private:
    static bool isGraphicQueueFamilySupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                              uint32_t& queueFamilyIndex);

    static bool isTransferQueueFamilySupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice,
                                                               uint32_t& queueFamilyIndex);

    static bool isPresentationSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                        const VkSurfaceKHR surface, 
                                                        uint32_t& queueFamilyIndex);

    static bool areDeviceExtensionsSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                             const std::vector<const char*>& deviceExtensions);

    static bool isSwapChainSupported(const VkPhysicalDevice physicalDevice, 
                                     const Surface& surface);

    bool isPhysicalDeviceSuitable(const VkPhysicalDevice physicalDevice, 
                                  const Surface& surface,
                                  uint32_t& graphicsSupportQueueFamilyIndex,
                                  uint32_t& transferSupportQueueFamilyIndex,
                                  uint32_t& presentationSupportQueueFamilyIndex) const;
    struct PhysicalDeviceInfo {
        VkPhysicalDevice mDevice = VK_NULL_HANDLE;
        uint32_t mGraphicsSupportQueueFamilyIndex = 0;
        uint32_t mTransferSupportQueueFamilyIndex = 0;
        uint32_t mPresentationSupportQueueFamilyIndex = 0;
    };
    std::vector<PhysicalDeviceInfo> getCandidateDevices(const AppInstance& appInstance,
                                                        const Surface& surface) const;

    void setPhysicalDevice(const std::vector<PhysicalDevice::PhysicalDeviceInfo>& candidateDevices);

    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;

    uint32_t mGraphicsSupportQueueFamilyIndex = 0;
    uint32_t mTransferSupportQueueFamilyIndex = 0;
    uint32_t mPresentationSupportQueueFamilyIndex = 0;

    std::vector<const char*> mDeviceExtensions;
};
}

#endif