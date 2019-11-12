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
    , mPhysicalDevice(new PhysicalDevice(*mInstance,
                                         *mSurface))
    , mLogicalDevice(new LogicalDevice(*mPhysicalDevice))
    , mSwapChain(new SwapChain(*mLogicalDevice, 
                               *mPhysicalDevice,
                               mWindow->width(),
                               mWindow->height(),
                               *mSurface))
    , mGraphicsCommandPool(new CommandPool(*mLogicalDevice, 
                                           mPhysicalDevice->graphicsSupportQueueFamilyIndex()))
    , mTransferCommandPool(new CommandPool(*mLogicalDevice,
                                           mPhysicalDevice->transferSupportQueueFamilyIndex(),
                                           VK_COMMAND_POOL_CREATE_TRANSIENT_BIT))
    , mPresentationCommandPool(new CommandPool(*mLogicalDevice, 
                                               mPhysicalDevice->presentationSupportQueueFamilyIndex()))
    , mImageSystem(*mLogicalDevice,
                   *mPhysicalDevice)
    , mShaderModuleSystem(*mLogicalDevice)
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

const PhysicalDevice&
SystemManager::physicalDevice() const {
    assert(mPhysicalDevice != nullptr);
    return *mPhysicalDevice;
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

ShaderModuleSystem&
SystemManager::shaderModuleSystem() {
    return mShaderModuleSystem;
}

ImageSystem&
SystemManager::imageSystem() {
    return mImageSystem;
}

}