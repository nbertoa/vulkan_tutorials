#ifndef UTILS_BASE_APP
#define UTILS_BASE_APP

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "CommandBuffers.h"
#include "CreatorDefinitions.h"
#include "Fences.h"
#include "FrameBuffers.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "Semaphores.h"
#include "SystemManager.h"

namespace vk {
class BaseApp {
public:
    BaseApp(const uint32_t windowWidth,
            const uint32_t windowHeight,
            const char* windowTitle,
            const RenderPassCreator& renderPassCreator,
            const GraphicsPipelineCreator& graphicsPipelineCreator);

    virtual void run();

protected:
    virtual void submitCommandBufferAndPresent();

    // The member variables declaration order is important because
    // the destruction order must be this.
    SystemManager mSystemManager;
    std::unique_ptr<RenderPass> mRenderPass;
    std::unique_ptr<GraphicsPipeline> mGraphicsPipeline;
    std::unique_ptr<FrameBuffers> mFrameBuffers;
    std::unique_ptr<CommandBuffers> mCommandBuffers;

    Semaphores mImageAvailableSemaphores;
    Semaphores mRenderFinishedSemaphores;
    Fences mFences;
};
}

#endif