#include "LogicalDevice.h"

#include <cassert>

#include "../DebugUtils.h"
#include "PhysicalDevice.h"

namespace vk {
VkDevice 
LogicalDevice::mLogicalDevice = VK_NULL_HANDLE;

VkQueue 
LogicalDevice::mGraphicsQueue = VK_NULL_HANDLE;

VkQueue 
LogicalDevice::mTransferQueue = VK_NULL_HANDLE;

VkQueue 
LogicalDevice::mPresentationQueue = VK_NULL_HANDLE;

void
LogicalDevice::initialize(const std::vector<const char*>& deviceExtensionNames) {
    assert(mLogicalDevice == VK_NULL_HANDLE);

    mLogicalDevice = createLogicalDevice(deviceExtensionNames);

    initQueues();
}

void
LogicalDevice::finalize() {
    assert(mLogicalDevice != VK_NULL_HANDLE);

    vkDestroyDevice(mLogicalDevice,
                    nullptr);
}

VkDevice
LogicalDevice::vkDevice() {
    assert(mLogicalDevice != VK_NULL_HANDLE);
    return mLogicalDevice;
}

VkQueue
LogicalDevice::graphicsQueue() {
    assert(mGraphicsQueue != VK_NULL_HANDLE);
    return mGraphicsQueue;
}

VkQueue
LogicalDevice::transferQueue() {
    assert(mTransferQueue != VK_NULL_HANDLE);
    return mTransferQueue;
}

VkQueue
LogicalDevice::presentationQueue() {
    assert(mPresentationQueue != VK_NULL_HANDLE);
    return mPresentationQueue;
}

VkDevice
LogicalDevice::createLogicalDevice(const std::vector<const char*>& deviceExtensionNames) {
    VkDevice logicalDevice;

    const std::vector<VkDeviceQueueCreateInfo> createInfoVector = 
        physicalDeviceQueuesCreateInfo();
    
    // VkDeviceCreateInfo:
    // - queueCreateInfoCount
    // - pQueueCreateInfos describes the queues that are requested to be created 
    //   along with the logical device.
    // - enabledExtensionCount is the number of device extensions to enable.
    // - ppEnabledExtensionNames to enable for the created device.
    // - pEnabledFeatures is an optional VkPhysicalDeviceFeatures structure containing 
    //   boolean indicators of all the features to be enabled.
    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
    physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo physicalDeviceCreateInfo = {};
    physicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;    
    physicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(createInfoVector.size());
    physicalDeviceCreateInfo.pQueueCreateInfos = createInfoVector.data();
    physicalDeviceCreateInfo.enabledLayerCount = 0; // deprecated and ignored
    physicalDeviceCreateInfo.ppEnabledLayerNames = nullptr; // deprecated and ignored
    physicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensionNames.size());
    physicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensionNames.data();
    physicalDeviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

    vkChecker(vkCreateDevice(PhysicalDevice::vkPhysicalDevice(),
                             &physicalDeviceCreateInfo, 
                             nullptr, 
                             &logicalDevice));
    assert(logicalDevice != VK_NULL_HANDLE);

    return logicalDevice;
}

std::vector<VkDeviceQueueCreateInfo>
LogicalDevice::physicalDeviceQueuesCreateInfo() {
    std::vector<VkDeviceQueueCreateInfo> createInfoVector;

    // Add graphics support queue family
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = PhysicalDevice::graphicsSupportQueueFamilyIndex();
    queueCreateInfo.queueCount = 1;
    const float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    createInfoVector.emplace_back(queueCreateInfo);

    // If graphics and presentation support queue family indices are different, 
    // we add a new create info for presentation queue.
    if (PhysicalDevice::graphicsSupportQueueFamilyIndex() !=
        PhysicalDevice::presentationSupportQueueFamilyIndex()) {
        queueCreateInfo.queueFamilyIndex = PhysicalDevice::presentationSupportQueueFamilyIndex();
        createInfoVector.emplace_back(queueCreateInfo);

        // If presentation and transfer support queue family indices are different,
        // then we add a new create info for transfer queue.
        if (PhysicalDevice::presentationSupportQueueFamilyIndex() !=
            PhysicalDevice::transferSupportQueueFamilyIndex()) {
            queueCreateInfo.queueFamilyIndex = PhysicalDevice::transferSupportQueueFamilyIndex();
            createInfoVector.emplace_back(queueCreateInfo);
        }
    } else if (PhysicalDevice::graphicsSupportQueueFamilyIndex() !=
               PhysicalDevice::transferSupportQueueFamilyIndex()) {
        // If graphics and transfer support queue family indices are different,
        // then we add a new create info for transfer queue.
        queueCreateInfo.queueFamilyIndex = PhysicalDevice::transferSupportQueueFamilyIndex();
        createInfoVector.emplace_back(queueCreateInfo);
    }

    return createInfoVector;
}

void
LogicalDevice::initQueues() {
    assert(mLogicalDevice != VK_NULL_HANDLE);

    vkGetDeviceQueue(mLogicalDevice,
                     PhysicalDevice::graphicsSupportQueueFamilyIndex(),
                     0,
                     &mGraphicsQueue);
    assert(mGraphicsQueue != VK_NULL_HANDLE);

    vkGetDeviceQueue(mLogicalDevice,
                     PhysicalDevice::transferSupportQueueFamilyIndex(),
                     0,
                     &mTransferQueue);
    assert(mTransferQueue != VK_NULL_HANDLE);

    vkGetDeviceQueue(mLogicalDevice,
                     PhysicalDevice::presentationSupportQueueFamilyIndex(),
                     0,
                     &mPresentationQueue);
    assert(mPresentationQueue != VK_NULL_HANDLE);
}

}