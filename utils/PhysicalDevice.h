#ifndef UTILS_PHYSICAL_DEVICE
#define UTILS_PHYSICAL_DEVICE

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class AppInstance;
class WindowSurface;

// VkPhysicalDevice wrapper to be able to create/destroy/get it easily.
// It represents a physical device installed in the system.
class PhysicalDevice {
public:
    PhysicalDevice(const AppInstance& appInstance, 
                   const WindowSurface& windowSurface);

    PhysicalDevice(const PhysicalDevice&) = delete;
    const PhysicalDevice& operator=(const PhysicalDevice&) = delete;

    VkPhysicalDevice vkPhysicalDevice() const { 
        assert(mPhysicalDevice != VK_NULL_HANDLE); 
        return mPhysicalDevice; 
    }

    uint32_t graphicsSupportQueueFamilyIndex() const { 
        assert(mPhysicalDevice != VK_NULL_HANDLE); 
        return mGraphicsSupportQueueFamilyIndex; 
    }

    uint32_t presentationSupportQueueFamilyIndex() const { 
        assert(mPhysicalDevice != VK_NULL_HANDLE); 
        return mPresentationSupportQueueFamilyIndex; 
    }

    const std::vector<const char*>& deviceExtensions() const { 
        assert(mPhysicalDevice != VK_NULL_HANDLE); 
        return mDeviceExtensions; 
    }

private:
    static bool isGraphicQueueFamilySupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                              uint32_t& queueFamilyIndex);

    static bool isPresentationSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                        const VkSurfaceKHR surface, 
                                                        uint32_t& queueFamilyIndex);

    static bool areDeviceExtensionsSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                             const std::vector<const char*>& deviceExtensions);

    static bool isSwapChainSupported(const VkPhysicalDevice physicalDevice, 
                                     const WindowSurface& windowSurface);

    bool isPhysicalDeviceSuitable(const VkPhysicalDevice physicalDevice, 
                                  const WindowSurface& windowSurface,
                                  uint32_t& graphicsSupportQueueFamilyIndex,
                                  uint32_t& presentationSupportQueueFamilyIndex) const;
    struct PhysicalDeviceInfo {
        VkPhysicalDevice mDevice = VK_NULL_HANDLE;
        uint32_t mGraphicsSupportQueueFamilyIndex = 0;
        uint32_t mPresentationSupportQueueFamilyIndex = 0;
    };
    std::vector<PhysicalDeviceInfo> getCandidateDevices(const AppInstance& appInstance,
                                                        const WindowSurface& windowSurface) const;

    void setPhysicalDevice(const std::vector<PhysicalDevice::PhysicalDeviceInfo>& candidateDevices);

    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;

    uint32_t mGraphicsSupportQueueFamilyIndex = 0;
    uint32_t mPresentationSupportQueueFamilyIndex = 0;

    std::vector<const char*> mDeviceExtensions;
};
}

#endif