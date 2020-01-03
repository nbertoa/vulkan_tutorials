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
private:
    static vk::UniqueCommandPool mGraphicsCommandPool;
    static vk::UniqueCommandPool mTransferCommandPool;
};
}

#endif