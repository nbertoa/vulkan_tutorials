#ifndef APP
#define APP

#include "Utils/SwapChain.h"
#include "Utils/pipeline/GraphicsPipeline.h"
#include "Utils/pipeline/PipelineStates.h" 
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
    initPipelineStates(vulkan::PipelineStates& pipelineStates) const;

    void
    initShaderStages(vulkan::ShaderStages& shaderStages);

    vulkan::SwapChain mSwapChain;

    vk::UniqueRenderPass mRenderPass;
    std::vector<vk::UniqueFramebuffer> mFrameBuffers;

    std::vector<vk::UniqueCommandBuffer> mCommandBuffers;

    std::unique_ptr<vulkan::GraphicsPipeline> mGraphicsPipeline;
    vulkan::PipelineStates mPipelineStates;

    std::unique_ptr<vulkan::Semaphores> mImageAvailableSemaphores;
    std::unique_ptr<vulkan::Semaphores> mRenderFinishedSemaphores;
    std::unique_ptr<vulkan::Fences> mFences;

    std::unique_ptr<vulkan::Buffer> mGpuVertexBuffer;
};

#endif 