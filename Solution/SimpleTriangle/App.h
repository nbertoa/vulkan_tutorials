#ifndef APP
#define APP

#include "Utils/Buffer.h"
#include "Utils/CommandBuffers.h"
#include "Utils/Fences.h"
#include "Utils/FrameBuffers.h"
#include "Utils/GraphicsPipeline.h"
#include "Utils/RenderPass.h"
#include "Utils/Semaphores.h"
#include "Utils/SystemManager.h"
#include "Utils/pipeline_stage/PipelineStates.h" 

namespace vk {
class ShaderStages;
}

class App {
public:
    App(const uint32_t windowWidth,
        const uint32_t windowHeight,
        const char* windowTitle);

    void
    run();

protected:
    void 
    initBuffers();

    void 
    recordCommandBuffers();

    void
    initGraphicsPipeline();

    void 
    initRenderPass();

    void
    submitCommandBufferAndPresent();

    void
    initFrameBuffers();

    void
    initCommandBuffers();

    void
    initSemaphoresAndFences();

    void
    initPipelineStates(vk::PipelineStates& pipelineStates) const;

    void
    initShaderStages(vk::ShaderStages& shaderStages);

    // The member variables declaration order is important because
    // the destruction order must be this.
    vk::SystemManager mSystemManager;
    std::unique_ptr<vk::RenderPass> mRenderPass;
    std::unique_ptr<vk::FrameBuffers> mFrameBuffers;
    std::unique_ptr<vk::CommandBuffers> mCommandBuffers;
    std::unique_ptr<vk::GraphicsPipeline> mGraphicsPipeline;

    vk::PipelineStates mPipelineStates;

    std::unique_ptr<vk::Semaphores> mImageAvailableSemaphores;
    std::unique_ptr<vk::Semaphores> mRenderFinishedSemaphores;
    std::unique_ptr<vk::Fences> mFences;

    std::unique_ptr<vk::Buffer> mGpuVertexBuffer;
};

#endif 