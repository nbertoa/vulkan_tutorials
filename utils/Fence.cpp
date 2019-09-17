#include "Fence.h"

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
Fence::Fence(const LogicalDevice& logicalDevice)
    : mLogicalDevice(logicalDevice) {
    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkChecker(vkCreateFence(mLogicalDevice.vkDevice(),
                            &createInfo,
                            nullptr,
                            &mFence));
}

Fence::Fence(Fence&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mFence(std::move(other.mFence)) {
    other.mFence = VK_NULL_HANDLE;
}

Fence::~Fence() {
    vkDestroyFence(mLogicalDevice.vkDevice(),
                   mFence,
                   nullptr);
}

void
Fence::wait() const {
    assert(mFence != VK_NULL_HANDLE);

    vkChecker(vkWaitForFences(mLogicalDevice.vkDevice(),
                              1,
                              &mFence,
                              VK_TRUE,
                              std::numeric_limits<uint64_t>::max()));
}

void
Fence::reset() const {
    assert(mFence != VK_NULL_HANDLE);

    vkChecker(vkResetFences(mLogicalDevice.vkDevice(),
                            1,
                            &mFence));
}

void 
Fence::waitAndReset() const {
    assert(mFence != VK_NULL_HANDLE);

    vkChecker(vkWaitForFences(mLogicalDevice.vkDevice(),
                              1,
                              &mFence,
                              VK_TRUE,
                              std::numeric_limits<uint64_t>::max()));
    vkChecker(vkResetFences(mLogicalDevice.vkDevice(),
                            1,
                            &mFence));
}
}