#include "SystemManager.h"

namespace vk {
SystemManager::SystemManager(const uint32_t windowWidth,
                             const uint32_t windowHeight,
                             const char* windowTitle)
    : mWindow(new Window(windowWidth, 
                         windowHeight, 
                         windowTitle))
    , mAppInstance(new AppInstance())
    , mSurface(new Surface(*mAppInstance, 
                           *mWindow))
    , mLogicalDevice(new LogicalDevice(*mAppInstance, 
                                       *mSurface))
    , mSwapChain(new SwapChain(*mLogicalDevice, 
                               *mWindow, 
                               *mSurface))
    , mGraphicsCommandPool(new CommandPool(*mLogicalDevice, 
                                           CommandPool::Type::GRAPHICS))
    , mPresentationCommandPool(new CommandPool(*mLogicalDevice, 
                                               CommandPool::Type::PRESENTATION))
{
}

}