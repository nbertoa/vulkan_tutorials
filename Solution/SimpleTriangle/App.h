#ifndef APP
#define APP

#include "Utils/SwapChain.h"
#include "Utils/command/CommandBuffers.h"
#include "Utils/command/CommandPool.h"
#include "Utils/pipeline/GraphicsPipeline.h"
#include "Utils/pipeline_stage/PipelineStates.h" 
#include "Utils/render_pass/RenderPass.h"
#include "Utils/resource/Buffer.h"
#include "Utils/sync/Fences.h"
#include "Utils/sync/Semaphores.h"

class App {
public:
    App();

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
    initPipelineStates(vk2::PipelineStates& pipelineStates) const;

    void
    initShaderStages(vk2::ShaderStages& shaderStages);

    vk2::SwapChain mSwapChain;

    std::unique_ptr<vk2::CommandPool> mGraphicsCommandPool;
    std::unique_ptr<vk2::CommandPool> mTransferCommandPool;

    std::unique_ptr<vk2::RenderPass> mRenderPass;
    std::vector<vk::UniqueFramebuffer> mFrameBuffers;

    std::unique_ptr<vk2::CommandBuffers> mCommandBuffers;

    std::unique_ptr<vk2::GraphicsPipeline> mGraphicsPipeline;
    vk2::PipelineStates mPipelineStates;

    std::unique_ptr<vk2::Semaphores> mImageAvailableSemaphores;
    std::unique_ptr<vk2::Semaphores> mRenderFinishedSemaphores;
    std::unique_ptr<vk2::Fences> mFences;

    std::unique_ptr<vk2::Buffer> mGpuVertexBuffer;
};

#endif 