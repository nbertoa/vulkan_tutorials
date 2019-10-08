#include "PhysicalDevice.h"

#include <cassert>

#include "DebugUtils.h"
#include "Instance.h"
#include "Surface.h"

namespace vk {
PhysicalDevice::PhysicalDevice(const Instance& instance,
                               const Surface& surface)
    : mDeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME} 
{
    setPhysicalDevice(getCandidateDevices(instance,
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

VkPhysicalDevice 
PhysicalDevice::vkPhysicalDevice() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPhysicalDevice;
}

uint32_t 
PhysicalDevice::graphicsSupportQueueFamilyIndex() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mGraphicsSupportQueueFamilyIndex;
}

uint32_t 
PhysicalDevice::transferSupportQueueFamilyIndex() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mTransferSupportQueueFamilyIndex;
}

uint32_t 
PhysicalDevice::presentationSupportQueueFamilyIndex() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPresentationSupportQueueFamilyIndex;
}

const std::vector<const char*>& 
PhysicalDevice::deviceExtensions() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mDeviceExtensions;
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
PhysicalDevice::isValidMemoryTypeIndex(const uint32_t memoryTypeIndex) const {
    return memoryTypeIndex != std::numeric_limits<uint32_t>::max();
}

std::vector<PhysicalDeviceData>
PhysicalDevice::getCandidateDevices(const Instance& instance,
                                    const Surface& surface) const {
    const std::vector<VkPhysicalDevice> physicalDevices = instance.physicalDevices();
    std::vector<PhysicalDeviceData> candidatePhysicalDevices;
    for (const VkPhysicalDevice& device : physicalDevices) {
        assert(device != VK_NULL_HANDLE);

        PhysicalDeviceData deviceData(device,
                                      surface,
                                      mDeviceExtensions);
        if (deviceData.isSupported()) {
            candidatePhysicalDevices.push_back(deviceData);
        }
    }

    return candidatePhysicalDevices;
}

void
PhysicalDevice::setPhysicalDevice(const std::vector<PhysicalDeviceData>& candidateDevices) {
    assert(candidateDevices.empty() == false && "There is no suitable physical device.");
    assert(mPhysicalDevice == VK_NULL_HANDLE);

    const PhysicalDeviceData* chosenDeviceData = &candidateDevices.front();

    // From all the suitable physical devices, we get the first that is a discrete GPU.
    // Otherwise, we get the first device in the list.
    for (const PhysicalDeviceData& deviceData : candidateDevices) {
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
}