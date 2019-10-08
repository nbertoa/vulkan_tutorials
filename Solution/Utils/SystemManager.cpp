#include "SystemManager.h"

#include <cassert>

namespace vk {
SystemManager::SystemManager(const uint32_t windowWidth,
                             const uint32_t windowHeight,
                             const char* windowTitle)
    : mWindow(new Window(windowWidth, 
                         windowHeight, 
                         windowTitle))
    , mInstance(new Instance())
    , mSurface(new Surface(*mInstance,
                           *mWindow))
    , mLogicalDevice(new LogicalDevice(*mInstance,
                                       *mSurface))
    , mSwapChain(new SwapChain(*mLogicalDevice, 
                               *mWindow, 
                               *mSurface))
    , mGraphicsCommandPool(new CommandPool(*mLogicalDevice, 
                                           CommandPool::Type::GRAPHICS))
    , mTransferCommandPool(new CommandPool(*mLogicalDevice,
                                           CommandPool::Type::TRANSFER,
                                           VK_COMMAND_POOL_CREATE_TRANSIENT_BIT))
    , mPresentationCommandPool(new CommandPool(*mLogicalDevice, 
                                               CommandPool::Type::PRESENTATION))
{
}

const Window& 
SystemManager::window() const {
    assert(mWindow != nullptr);
    return *mWindow;
}

const Instance&
SystemManager::instance() const {
    assert(mInstance != nullptr);
    return *mInstance;
}

const Surface& 
SystemManager::surface() const {
    assert(mSurface != nullptr);
    return *mSurface;
}

const LogicalDevice& 
SystemManager::logicalDevice() const {
    assert(mLogicalDevice != nullptr);
    return *mLogicalDevice;
}

SwapChain& 
SystemManager::swapChain() const {
    assert(mSwapChain != nullptr);
    return *mSwapChain;
}

const CommandPool& 
SystemManager::graphicsCommandPool() const {
    assert(mGraphicsCommandPool != nullptr);
    return *mGraphicsCommandPool;
}

const CommandPool& 
SystemManager::transferCommandPool() const {
    assert(mTransferCommandPool != nullptr);
    return *mTransferCommandPool;
}

const CommandPool& 
SystemManager::presentationCommandPool() const {
    assert(mPresentationCommandPool != nullptr);
    return *mPresentationCommandPool;
}

}