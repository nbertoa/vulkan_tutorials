#ifndef UTILS_SYSTEM_MANAGER
#define UTILS_SYSTEM_MANAGER

#include <cassert>
#include <memory>
#include <vulkan/vulkan.h>

#include "utils/AppInstance.h"
#include "utils/LogicalDevice.h"
#include "utils/SwapChain.h"
#include "utils/Window.h"
#include "utils/WindowSurface.h"

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

private:
    // The member variables declaration order is important because
    // the destruction order must be this.
    std::unique_ptr<vk::Window> mWindow;
    std::unique_ptr<vk::AppInstance> mAppInstance;
    std::unique_ptr<vk::WindowSurface> mWindowSurface;
    std::unique_ptr<vk::LogicalDevice> mLogicalDevice;
    std::unique_ptr<vk::SwapChain> mSwapChain;
};
}

#endif
