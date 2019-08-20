#ifndef UTILS_SYSTEM_MANAGER
#define UTILS_SYSTEM_MANAGER

#include <cassert>
#include <memory>
#include <vulkan/vulkan.h>

#include "AppInstance.h"
#include "CommandPool.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "Window.h"
#include "WindowSurface.h"

namespace vk {
class SystemManager {
public:
    SystemManager(const uint32_t windowWidth,
                  const uint32_t windowHeight,
                  const char* windowTitle);

    SystemManager(const SystemManager&) = delete;
    const SystemManager& operator=(const SystemManager&) = delete;

    const Window& window() const {
        assert(mWindow != nullptr);
        return *mWindow;
    }

    const AppInstance& appInstance() const {
        assert(mAppInstance != nullptr);
        return *mAppInstance;
    }

    const WindowSurface& windowSurface() const {
        assert(mWindowSurface != nullptr);
        return *mWindowSurface;
    }

    const LogicalDevice& logicalDevice() const {
        assert(mLogicalDevice != nullptr);
        return *mLogicalDevice;
    }

    const SwapChain& swapChain() const {
        assert(mSwapChain != nullptr);
        return *mSwapChain;
    }

    const CommandPool& graphicsCommandPool() const {
        assert(mGraphicsCommandPool != nullptr);
        return *mGraphicsCommandPool;
    }

    const CommandPool& presentationCommandPool() const {
        assert(mPresentationCommandPool != nullptr);
        return *mPresentationCommandPool;
    }

private:
    // The member variables declaration order is important because
    // the destruction order must be this.
    std::unique_ptr<Window> mWindow;
    std::unique_ptr<AppInstance> mAppInstance;
    std::unique_ptr<WindowSurface> mWindowSurface;
    std::unique_ptr<LogicalDevice> mLogicalDevice;
    std::unique_ptr<SwapChain> mSwapChain;
    std::unique_ptr<CommandPool> mGraphicsCommandPool;
    std::unique_ptr<CommandPool> mPresentationCommandPool;
};
}

#endif
