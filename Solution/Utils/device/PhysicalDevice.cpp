#include "PhysicalDevice.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../Instance.h"

namespace vk2 {
VkPhysicalDevice 
PhysicalDevice::mPhysicalDevice = VK_NULL_HANDLE;

uint32_t 
PhysicalDevice::mGraphicsSupportQueueFamilyIndex = 0;

uint32_t 
PhysicalDevice::mTransferSupportQueueFamilyIndex = 0;

uint32_t 
PhysicalDevice::mPresentationSupportQueueFamilyIndex = 0;

std::vector<const char*> mDeviceExtensions;

void
PhysicalDevice::initialize(const std::vector<const char*>& deviceExtensionNames) {
    assert(deviceExtensionNames.empty() == false);

    const std::vector<PhysicalDeviceData> candidatePhysicalDevices = 
        Instance::getCandidatePhysicalDevices(deviceExtensionNames);

    const PhysicalDeviceData* chosenDeviceData = &candidatePhysicalDevices.front();

    // From all the suitable physical devices, we get the first that is a discrete GPU.
    // Otherwise, we get the first device in the list.
    for (const PhysicalDeviceData& deviceData : candidatePhysicalDevices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(deviceData.vkPhysicalDevice(),
                                      &deviceProperties);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            chosenDeviceData = &deviceData;
            break;
        }
    }

    mPhysicalDevice = chosenDeviceData->vkPhysicalDevice();
    mGraphicsSupportQueueFamilyIndex = chosenDeviceData->graphicsSupportQueueFamilyIndex();
    mTransferSupportQueueFamilyIndex = chosenDeviceData->transferSupportQueueFamilyIndex();
    mPresentationSupportQueueFamilyIndex = chosenDeviceData->presentationSupportQueueFamilyIndex();
}

void
PhysicalDevice::finalize() {

}

VkPhysicalDevice 
PhysicalDevice::vkPhysicalDevice() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPhysicalDevice;
}

uint32_t 
PhysicalDevice::graphicsSupportQueueFamilyIndex() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mGraphicsSupportQueueFamilyIndex;
}

uint32_t 
PhysicalDevice::transferSupportQueueFamilyIndex() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mTransferSupportQueueFamilyIndex;
}

uint32_t 
PhysicalDevice::presentationSupportQueueFamilyIndex() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPresentationSupportQueueFamilyIndex;
}

uint32_t 
PhysicalDevice::memoryTypeIndex(const uint32_t memoryTypeFilter,
                                const VkMemoryPropertyFlags memoryPropertyFlags) {

    uint32_t typeIndex = std::numeric_limits<uint32_t>::max();

    // Each physical device can then be queried for its capabilities, 
    // including its queueand queue family properties.
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice,
                                        &memoryProperties);

    // memoryTypeFilter is used to specify the bit field of memory types
    // that are suitable.
    // That means that we can find the index of a suitable memory type
    // by simply iterating over them and checking if the corresponding
    // bit is set to 1.
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        // We can find the index of a suitable memory type
        // by simply iterating over them and checking if the corresponding
        // bit is set to 1.
        if (memoryTypeFilter & (1 << i)) {
            const VkMemoryPropertyFlags currentMemoryPropertyFlags = 
                memoryProperties.memoryTypes[i].propertyFlags;

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
PhysicalDevice::isValidMemoryTypeIndex(const uint32_t memoryTypeIndex) {
    return memoryTypeIndex != std::numeric_limits<uint32_t>::max();
}
}