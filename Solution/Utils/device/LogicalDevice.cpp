#include "LogicalDevice.h"

#include <cassert>

#include "../DebugUtils.h"
#include "PhysicalDevice.h"

namespace vk {
LogicalDevice::LogicalDevice(const PhysicalDevice& physicalDevice) 
    : mLogicalDevice(createLogicalDevice(physicalDevice))
{
    initQueues(physicalDevice);
}

LogicalDevice::~LogicalDevice() {
    vkDestroyDevice(mLogicalDevice,
                    nullptr);
}

LogicalDevice::LogicalDevice(LogicalDevice&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mGraphicsQueue(other.mGraphicsQueue)
    , mPresentationQueue(other.mPresentationQueue)
{
    other.mLogicalDevice = VK_NULL_HANDLE;
    other.mGraphicsQueue = VK_NULL_HANDLE;
    other.mPresentationQueue = VK_NULL_HANDLE;
}

VkDevice
LogicalDevice::vkDevice() const {
    assert(mLogicalDevice != VK_NULL_HANDLE);
    return mLogicalDevice;
}

VkQueue
LogicalDevice::graphicsQueue() const {
    assert(mGraphicsQueue != VK_NULL_HANDLE);
    return mGraphicsQueue;
}

VkQueue
LogicalDevice::transferQueue() const {
    assert(mTransferQueue != VK_NULL_HANDLE);
    return mTransferQueue;
}

VkQueue
LogicalDevice::presentationQueue() const {
    assert(mPresentationQueue != VK_NULL_HANDLE);
    return mPresentationQueue;
}

VkDevice
LogicalDevice::createLogicalDevice(const PhysicalDevice& physicalDevice) {
    VkDevice logicalDevice;

    const std::vector<VkDeviceQueueCreateInfo> createInfoVector = physicalDeviceQueuesCreateInfo(physicalDevice);
    const std::vector<const char*>& deviceExtensions = physicalDevice.deviceExtensionNames();
    
    // VkDeviceCreateInfo:
    // - queueCreateInfoCount
    // - pQueueCreateInfos describes the queues that are requested to be created along with the logical device.
    // - enabledExtensionCount is the number of device extensions to enable.
    // - ppEnabledExtensionNames to enable for the created device.
    // - pEnabledFeatures is an optional VkPhysicalDeviceFeatures structure containing 
    //   boolean indicators of all the features to be enabled.
    VkDeviceCreateInfo physicalDeviceCreateInfo = {};
    physicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;    
    physicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(createInfoVector.size());
    physicalDeviceCreateInfo.pQueueCreateInfos = createInfoVector.data();
    physicalDeviceCreateInfo.enabledLayerCount = 0; // deprecated and ignored
    physicalDeviceCreateInfo.ppEnabledLayerNames = nullptr; // deprecated and ignored
    physicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    physicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    physicalDeviceCreateInfo.pEnabledFeatures = nullptr;

    vkChecker(vkCreateDevice(physicalDevice.vkPhysicalDevice(),
                             &physicalDeviceCreateInfo, 
                             nullptr, 
                             &logicalDevice));
    assert(logicalDevice != VK_NULL_HANDLE);

    return logicalDevice;
}

std::vector<VkDeviceQueueCreateInfo>
LogicalDevice::physicalDeviceQueuesCreateInfo(const PhysicalDevice& physicalDevice) {
    assert(physicalDevice.vkPhysicalDevice() != VK_NULL_HANDLE);

    std::vector<VkDeviceQueueCreateInfo> createInfoVector;

    // Add graphics support queue family
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = physicalDevice.graphicsSupportQueueFamilyIndex();
    queueCreateInfo.queueCount = 1;
    const float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    createInfoVector.emplace_back(queueCreateInfo);

    // If graphics and presentation support queue family indices are different, 
    // we add a new create info for presentation queue.
    if (physicalDevice.graphicsSupportQueueFamilyIndex() != 
        physicalDevice.presentationSupportQueueFamilyIndex()) {
        queueCreateInfo.queueFamilyIndex = physicalDevice.presentationSupportQueueFamilyIndex();
        createInfoVector.emplace_back(queueCreateInfo);

        // If presentation and transfer support queue family indices are different,
        // then we add a new create info for transfer queue.
        if (physicalDevice.presentationSupportQueueFamilyIndex() !=
            physicalDevice.transferSupportQueueFamilyIndex()) {
            queueCreateInfo.queueFamilyIndex = physicalDevice.transferSupportQueueFamilyIndex();
            createInfoVector.emplace_back(queueCreateInfo);
        }
    } else if (physicalDevice.graphicsSupportQueueFamilyIndex() !=
               physicalDevice.transferSupportQueueFamilyIndex()) {
        // If graphics and transfer support queue family indices are different,
        // then we add a new create info for transfer queue.
        queueCreateInfo.queueFamilyIndex = physicalDevice.transferSupportQueueFamilyIndex();
        createInfoVector.emplace_back(queueCreateInfo);
    }

    return createInfoVector;
}

void
LogicalDevice::initQueues(const PhysicalDevice& physicalDevice) {
    assert(mLogicalDevice != VK_NULL_HANDLE);

    vkGetDeviceQueue(mLogicalDevice,
                     physicalDevice.graphicsSupportQueueFamilyIndex(),
                     0,
                     &mGraphicsQueue);
    assert(mGraphicsQueue != VK_NULL_HANDLE);

    vkGetDeviceQueue(mLogicalDevice,
                     physicalDevice.transferSupportQueueFamilyIndex(),
                     0,
                     &mTransferQueue);
    assert(mTransferQueue != VK_NULL_HANDLE);

    vkGetDeviceQueue(mLogicalDevice,
                     physicalDevice.presentationSupportQueueFamilyIndex(),
                     0,
                     &mPresentationQueue);
    assert(mPresentationQueue != VK_NULL_HANDLE);
}

}