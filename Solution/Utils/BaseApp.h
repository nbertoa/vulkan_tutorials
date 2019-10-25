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
#include "pipeline_stage/PipelineStates.h"

namespace vk {
class BaseApp {
public:
    BaseApp(const uint32_t windowWidth,
            const uint32_t windowHeight,
            const char* windowTitle,
            const RenderPassCreator& renderPassCreator);

    virtual 
    void run();

protected:
    // Inherited classes should implement this method.
    // It is responsible to do everything just after the 
    // event processing and just before the command buffer
    // submission.
    virtual 
    void processCurrentFrame() {};

    // Inherited classes should implement this method
    // and call it in the constructor.
    virtual 
    void createGraphicsPipeline() = 0;

    virtual 
    void submitCommandBufferAndPresent();

    // The member variables declaration order is important because
    // the destruction order must be this.
    SystemManager mSystemManager;
    std::unique_ptr<RenderPass> mRenderPass;    
    std::unique_ptr<FrameBuffers> mFrameBuffers;
    std::unique_ptr<CommandBuffers> mCommandBuffers;
    std::unique_ptr<GraphicsPipeline> mGraphicsPipeline;

    PipelineStates mPipelineStates;

    Semaphores mImageAvailableSemaphores;
    Semaphores mRenderFinishedSemaphores;
    Fences mFences;
};
}

#endif
