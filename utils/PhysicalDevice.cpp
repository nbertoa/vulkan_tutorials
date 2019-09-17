#include "PhysicalDevice.h"

#include <iostream>
#include <unordered_set>
#include <vector>

#include "AppInstance.h"
#include "DebugUtils.h"
#include "Surface.h"

namespace vk {
PhysicalDevice::PhysicalDevice(const AppInstance& appInstance,
                               const Surface& surface)
    : mDeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME} 
{
    setPhysicalDevice(getCandidateDevices(appInstance,
                                          surface));
}
PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
    : mPhysicalDevice(other.mPhysicalDevice)
    , mGraphicsSupportQueueFamilyIndex(other.mGraphicsSupportQueueFamilyIndex)
    , mPresentationSupportQueueFamilyIndex(other.mPresentationSupportQueueFamilyIndex)
    , mDeviceExtensions(std::move(other.mDeviceExtensions))
{
    other.mPhysicalDevice = VK_NULL_HANDLE;
}

uint32_t 
PhysicalDevice::memoryTypeIndex(const uint32_t memoryTypeFilter,
                                const VkMemoryPropertyFlags memoryPropertyFlags) const {

    uint32_t typeIndex = std::numeric_limits<uint32_t>::max();

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice,
                                        &memoryProperties);

    // We have two arrays memoryTypes and memoryHeaps in the 
    // memory properties structure.
    // Memory heaps are distinct memory resources like dedicated VRAM
    // and swap space in RAM for when VRAM runs out.
    // The different types of memory exist within these heaps.
    //
    // memoryTypeFilter will be used to specify the bit field of memory types
    // that are suitable.
    // That means that we can find the index of a suitable memory type
    // by simply iterating over them and checking if the corresponding
    // bit is set to 1.
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        // We can find the index of a suitable memory type
        // by simply iterating over them and checking if the corresponding
        // bit is set to 1.
        if (memoryTypeFilter & (1 << i)) {
            // The memoryTypes array consists of VkMemoryType structs that specify
            // the heap and properties of each type of memory.
            // The properties of the memory, like being able to map it so we can 
            // write it to it from the CPU.
            const VkMemoryPropertyFlags currentMemoryPropertyFlags = memoryProperties.memoryTypes[i].propertyFlags;

            // We may have more than one desiderable property, so we should
            // check if the result of the bitwise AND is not just non-zero,
            // but equal to the desired properties bit field.
            // If there is a memory type suitable for the buffer that also
            // has all of the properties we need, then we return its index,
            // otherwise, std::numeric_limits<uint32_t>::max().
            if ((currentMemoryPropertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
                typeIndex = i;
                break;
            }
        }
    }

    return typeIndex;
}

bool
PhysicalDevice::isGraphicQueueFamilySupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                              uint32_t& queueFamilyIndex) {
    assert(physicalDevice != VK_NULL_HANDLE);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, 
                                             &queueFamilyCount, 
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, 
                                             &queueFamilyCount, 
                                             queueFamilyProperties.data());

    queueFamilyIndex = 0;
    for (const VkQueueFamilyProperties& queueFamilyProperty : queueFamilyProperties) {
        if (queueFamilyProperty.queueCount > 0 && 
            queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return true;
        }

        ++queueFamilyIndex;
    }

    return false;
}

bool
PhysicalDevice::isTransferQueueFamilySupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice,
                                                              uint32_t& queueFamilyIndex) {
    assert(physicalDevice != VK_NULL_HANDLE);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
                                             &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
                                             &queueFamilyCount,
                                             queueFamilyProperties.data());

    queueFamilyIndex = 0;
    for (const VkQueueFamilyProperties& queueFamilyProperty : queueFamilyProperties) {
        if (queueFamilyProperty.queueCount > 0 &&
            queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            return true;
        }

        ++queueFamilyIndex;
    }

    return false;
}

bool
PhysicalDevice::isPresentationSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                        const VkSurfaceKHR surface, 
                                                        uint32_t& queueFamilyIndex) {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(surface != VK_NULL_HANDLE);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, 
                                             &queueFamilyCount, 
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, 
                                             &queueFamilyCount, 
                                             queueFamilyProperties.data());

    queueFamilyIndex = 0;
    for (const VkQueueFamilyProperties& queueFamilyProperty : queueFamilyProperties) {
        if (queueFamilyProperty.queueCount > 0) {
            VkBool32 supported = false;
            vkChecker(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, 
                                                           queueFamilyIndex, 
                                                           surface,
                                                           &supported));
            if (supported) {
                return true;
            }
        }

        ++queueFamilyIndex;
    }

    return false;
}

bool
PhysicalDevice::areDeviceExtensionsSupportedByPhysicalDevice(const VkPhysicalDevice physicalDevice, 
                                                             const std::vector<const char*>& deviceExtensions) {
    assert(physicalDevice != VK_NULL_HANDLE);

    uint32_t extensionCount;
    vkChecker(vkEnumerateDeviceExtensionProperties(physicalDevice, 
                                                   nullptr,
                                                   &extensionCount,
                                                   nullptr));

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkChecker(vkEnumerateDeviceExtensionProperties(physicalDevice, 
                                                   nullptr,
                                                   &extensionCount, 
                                                   availableExtensions.data()));

    std::unordered_set<std::string> requiredExtensions(deviceExtensions.begin(), 
                                                       deviceExtensions.end());

    for (const VkExtensionProperties& extensionProperty : availableExtensions) {
        requiredExtensions.erase(extensionProperty.extensionName);
    }

#ifndef NDEBUG // Debug
    for (const std::string& extensionProperty : requiredExtensions) {
        std::cerr << "Unsupported extension property: " << extensionProperty << std::endl;
    }
#endif

    return requiredExtensions.empty();
}

bool
PhysicalDevice::isSwapChainSupported(const VkPhysicalDevice physicalDevice, 
                                     const Surface& surface) {
    assert(physicalDevice != VK_NULL_HANDLE);
    return surface.formats(physicalDevice).empty() == false && 
           surface.presentModes(physicalDevice).empty() == false;
}

bool
PhysicalDevice::isPhysicalDeviceSuitable(const VkPhysicalDevice physicalDevice, 
                                         const Surface& surface,
                                         uint32_t& graphicsSupportQueueFamilyIndex,
                                         uint32_t& transferSupportQueueFamilyIndex,
                                         uint32_t& presentationSupportQueueFamilyIndex) const {
    assert(physicalDevice != VK_NULL_HANDLE);

    return isGraphicQueueFamilySupportedByPhysicalDevice(physicalDevice, 
                                                         graphicsSupportQueueFamilyIndex) &&
           isTransferQueueFamilySupportedByPhysicalDevice(physicalDevice,
                                                          transferSupportQueueFamilyIndex) &&
           isPresentationSupportedByPhysicalDevice(physicalDevice, 
                                                   surface.vkSurface(), 
                                                   presentationSupportQueueFamilyIndex) &&
           areDeviceExtensionsSupportedByPhysicalDevice(physicalDevice, 
                                                        mDeviceExtensions) &&
           isSwapChainSupported(physicalDevice, 
                                surface);
}

std::vector<PhysicalDevice::PhysicalDeviceInfo>
PhysicalDevice::getCandidateDevices(const AppInstance& appInstance,
                                    const Surface& surface) const {
    uint32_t physicalDeviceCount = 0;
    vkChecker(vkEnumeratePhysicalDevices(appInstance.vkInstance(), 
                                         &physicalDeviceCount, 
                                         nullptr));
    assert(physicalDeviceCount > 0);

    // Get all the suitable physical devices
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkChecker(vkEnumeratePhysicalDevices(appInstance.vkInstance(),
                                         &physicalDeviceCount,
                                         physicalDevices.data()));
    std::vector<PhysicalDeviceInfo> candidatePhysicalDevices;
    for (const VkPhysicalDevice& device : physicalDevices) {
        assert(device != VK_NULL_HANDLE);

        uint32_t graphicsSupportQueueFamilyIndex;
        uint32_t transferSupportQueueFamilyIndex;
        uint32_t presentationSupportQueueFamilyIndex;
        if (isPhysicalDeviceSuitable(device, 
                                     surface,
                                     graphicsSupportQueueFamilyIndex,
                                     transferSupportQueueFamilyIndex,
                                     presentationSupportQueueFamilyIndex)) {
            PhysicalDeviceInfo physicalDeviceInfo;
            physicalDeviceInfo.mDevice = device;
            physicalDeviceInfo.mGraphicsSupportQueueFamilyIndex = graphicsSupportQueueFamilyIndex;
            physicalDeviceInfo.mTransferSupportQueueFamilyIndex = transferSupportQueueFamilyIndex;
            physicalDeviceInfo.mPresentationSupportQueueFamilyIndex = presentationSupportQueueFamilyIndex;
            candidatePhysicalDevices.push_back(physicalDeviceInfo);
        }
    }

    return candidatePhysicalDevices;
}

void
PhysicalDevice::setPhysicalDevice(const std::vector<PhysicalDevice::PhysicalDeviceInfo>& candidateDevices) {
    assert(candidateDevices.empty() == false && "There is no suitable physical device.");
    assert(mPhysicalDevice == VK_NULL_HANDLE);

    // From all the suitable physical devices, we get the first that is a discrete GPU.
    // Otherwise, we get the first device in the list.
    for (const PhysicalDeviceInfo& deviceInfo : candidateDevices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(deviceInfo.mDevice, &deviceProperties);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            mPhysicalDevice = deviceInfo.mDevice;
            mGraphicsSupportQueueFamilyIndex = deviceInfo.mGraphicsSupportQueueFamilyIndex;
            mTransferSupportQueueFamilyIndex = deviceInfo.mTransferSupportQueueFamilyIndex;
            mPresentationSupportQueueFamilyIndex = deviceInfo.mPresentationSupportQueueFamilyIndex;
            break;
        }
    }

    if (mPhysicalDevice == VK_NULL_HANDLE) {
        const PhysicalDeviceInfo& deviceInfo = candidateDevices.front();
        mPhysicalDevice = deviceInfo.mDevice;
        mGraphicsSupportQueueFamilyIndex = deviceInfo.mGraphicsSupportQueueFamilyIndex;
        mTransferSupportQueueFamilyIndex = deviceInfo.mTransferSupportQueueFamilyIndex;
        mPresentationSupportQueueFamilyIndex = deviceInfo.mPresentationSupportQueueFamilyIndex;
    }
}
}