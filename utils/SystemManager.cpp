#include "SystemManager.h"

namespace vk {
SystemManager::SystemManager(const uint32_t windowWidth,
                             const uint32_t windowHeight,
                             const char* windowTitle)
    : mWindow(new Window(windowWidth, windowHeight, windowTitle))
    , mAppInstance(new AppInstance())
    , mWindowSurface(new WindowSurface(*mAppInstance, *mWindow))
    , mLogicalDevice(new LogicalDevice(*mAppInstance, *mWindowSurface))
    , mSwapChain(new SwapChain(*mLogicalDevice, *mWindow, *mWindowSurface)) 
    , mGraphicsCommandPool(new CommandPool(*mLogicalDevice, CommandPool::Type::GRAPHICS))
    , mPresentationCommandPool(new CommandPool(*mLogicalDevice, CommandPool::Type::PRESENTATION))
{
}

}