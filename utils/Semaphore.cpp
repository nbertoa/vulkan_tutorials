#include "Semaphore.h"

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
Semaphore::Semaphore(const LogicalDevice& logicalDevice)
    : mLogicalDevice(logicalDevice)
{
    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkChecker(vkCreateSemaphore(mLogicalDevice.vkDevice(),
                                &createInfo,
                                nullptr,
                                &mSemaphore));
}

Semaphore::Semaphore(Semaphore&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mSemaphore(std::move(other.mSemaphore))
{
    other.mSemaphore = VK_NULL_HANDLE;
}

Semaphore::~Semaphore() {
    vkDestroySemaphore(mLogicalDevice.vkDevice(),
                       mSemaphore,
                       nullptr);
}
}