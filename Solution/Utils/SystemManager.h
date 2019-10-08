#ifndef UTILS_SYSTEM_MANAGER
#define UTILS_SYSTEM_MANAGER

#include <memory>
#include <vulkan/vulkan.h>

#include "CommandPool.h"
#include "Instance.h"
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

    const Window& window() const;
    const Instance& instance() const;
    const Surface& surface() const;
    const LogicalDevice& logicalDevice() const;
    SwapChain& swapChain() const;
    const CommandPool& graphicsCommandPool() const;
    const CommandPool& transferCommandPool() const;
    const CommandPool& presentationCommandPool() const;

private:
    // The member variables declaration order is important because
    // the destruction order must be this.
    std::unique_ptr<Window> mWindow;
    std::unique_ptr<Instance> mInstance;
    std::unique_ptr<Surface> mSurface;
    std::unique_ptr<LogicalDevice> mLogicalDevice;
    std::unique_ptr<SwapChain> mSwapChain;
    std::unique_ptr<CommandPool> mGraphicsCommandPool;
    std::unique_ptr<CommandPool> mTransferCommandPool;
    std::unique_ptr<CommandPool> mPresentationCommandPool;
};
}

#endif
