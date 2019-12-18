#ifndef APP
#define APP

#include "MatrixUBO.h"

#include "Utils/SwapChain.h"
#include "Utils/pipeline/GraphicsPipeline.h"
#include "Utils/pipeline/PipelineStates.h"
#include "Utils/resource/Buffer.h"
#include "Utils/sync/Fences.h"
#include "Utils/sync/Semaphores.h"

namespace vulkan {
class ShaderStages;
}

class App {
public:
    App();

    void
    run();

protected:
    void 
    processCurrentFrame();

    void
    initDescriptorSets();

    void 
    initBuffers();

    void 
    initVertexBuffer();

    void 
    initIndexBuffer();

    void 
    initUniformBuffers();

    void 
    recordCommandBuffers();

    void
    initGraphicsPipeline();

    void
    initPipelineStates(vulkan::PipelineStates& pipelineStates) const;

    void
    initShaderStages(vulkan::ShaderStages& shaderStages);

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

    vulkan::SwapChain mSwapChain;

    vk::UniqueCommandPool mGraphicsCommandPool;
    vk::UniqueCommandPool mTransferCommandPool;

    vk::UniqueRenderPass mRenderPass;
    std::vector<vk::UniqueFramebuffer> mFrameBuffers;

    std::vector<vk::UniqueCommandBuffer> mCommandBuffers;
    std::unique_ptr<vulkan::GraphicsPipeline> mGraphicsPipeline;

    vulkan::PipelineStates mPipelineStates;

    std::unique_ptr<vulkan::Semaphores> mImageAvailableSemaphores;
    std::unique_ptr<vulkan::Semaphores> mRenderFinishedSemaphores;
    std::unique_ptr<vulkan::Fences> mFences;

    std::unique_ptr<vulkan::Buffer> mGpuVertexBuffer;
    std::unique_ptr<vulkan::Buffer> mGpuIndexBuffer;

    std::vector<vulkan::Buffer> mUniformBuffers;
    vk::UniqueDescriptorPool mDescriptorPool;
    MatrixUBO mMatrixUBO;
    vk::UniqueDescriptorSetLayout mDescriptorSetLayout;
    std::vector<vk::DescriptorSet> mDescriptorSets;
    
};

#endif 