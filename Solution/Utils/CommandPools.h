#ifndef UTILS_COMMAND_POOLS
#define UTILS_COMMAND_POOLS

#include <vulkan/vulkan.hpp>

namespace vulkan {
class CommandPools {
public:
    static void
    initialize();

    static void 
    finalize();

    static vk::CommandPool
    graphicsCommandPool();

    static vk::CommandPool
    transferCommandPool();

    // These methods are used for transfer operations.
    static vk::UniqueCommandBuffer
    beginOneTimeSubmitCommandBuffer();
    static void
    endAndWaitOneTimeSubmitCommandBuffer(vk::CommandBuffer commandBuffer);

private:
    static vk::UniqueCommandPool mGraphicsCommandPool;
    static vk::UniqueCommandPool mTransferCommandPool;
};
}

#endif