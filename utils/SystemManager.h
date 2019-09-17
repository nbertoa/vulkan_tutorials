#ifndef UTILS_SYSTEM_MANAGER
#define UTILS_SYSTEM_MANAGER

#include <cassert>
#include <memory>
#include <vulkan/vulkan.h>

#include "AppInstance.h"
#include "CommandPool.h"
#include "LogicalDevice.h"
#include "Surface.h"
#include "SwapChain.h"
#include "Window.h"

namespace vk {
class SystemManager {
public:
    SystemManager(const uint32_t windowWidth,
                  const uint32_t windowHeight,
                  const char* windowTitle);

    SystemManager(const SystemManager&) = delete;
    const SystemManager& operator=(const SystemManager&) = delete;
    SystemManager(SystemManager&&) = delete;
    SystemManager& operator=(SystemManager&&) = delete;

    const Window& window() const {
        assert(mWindow != nullptr);
        return *mWindow;
    }

    const AppInstance& appInstance() const {
        assert(mAppInstance != nullptr);
        return *mAppInstance;
    }

    const Surface& surface() const {
        assert(mSurface != nullptr);
        return *mSurface;
    }

    const LogicalDevice& logicalDevice() const {
        assert(mLogicalDevice != nullptr);
        return *mLogicalDevice;
    }

    SwapChain& swapChain() const {
        assert(mSwapChain != nullptr);
        return *mSwapChain;
    }

    const CommandPool& graphicsCommandPool() const {
        assert(mGraphicsCommandPool != nullptr);
        return *mGraphicsCommandPool;
    }

    const CommandPool& transferCommandPool() const {
        assert(mTransferCommandPool != nullptr);
        return *mTransferCommandPool;
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
    std::unique_ptr<Surface> mSurface;
    std::unique_ptr<LogicalDevice> mLogicalDevice;
    std::unique_ptr<SwapChain> mSwapChain;
    std::unique_ptr<CommandPool> mGraphicsCommandPool;
    std::unique_ptr<CommandPool> mTransferCommandPool;
    std::unique_ptr<CommandPool> mPresentationCommandPool;
};
}

#endif
