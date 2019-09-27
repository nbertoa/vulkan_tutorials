#ifndef UTILS_CREATOR_DEFINITIONS
#define UTILS_CREATOR_DEFINITIONS

#include <functional>

namespace vk {
class GraphicsPipeline;
class LogicalDevice;
class RenderPass;
class SwapChain;

// Methods used as factories by BaseApp to be able
// to polymorphically create entities (pipeline, render pass, etc)
using GraphicsPipelineCreator = std::function<GraphicsPipeline*(const LogicalDevice& logicalDevice,
                                                                const SwapChain& swapChain,
                                                                const RenderPass& renderPass)>;

using RenderPassCreator = std::function<RenderPass*(const LogicalDevice&,
                                                    const SwapChain&)>;
}

#endif 