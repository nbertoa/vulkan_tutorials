#ifndef APP
#define APP

#include <vulkan/vulkan.hpp>

#include "MatrixUBO.h"

#include "Utils/SwapChain.h"
#include "Utils/command/CommandBuffers.h"
#include "Utils/command/CommandPool.h"
#include "Utils/pipeline/GraphicsPipeline.h"
#include "Utils/pipeline_stage/PipelineStates.h"
#include "Utils/resource/Buffers.h"
#include "Utils/resource/ImageView.h"
#include "Utils/sync/Fences.h"
#include "Utils/sync/Semaphores.h"

namespace vk2 {
class ShaderStages;
}

class App {
public:
    App();

    void
    run();

protected:
    void 
    updateUniformBuffers();

    void
    initDescriptorSets();

    void 
    initImages();

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
    initPipelineStates(vk2::PipelineStates& pipelineStates) const;

    void
    initShaderStages(vk2::ShaderStages& shaderStages);

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

    vk2::SwapChain mSwapChain;

    std::unique_ptr<vk2::CommandPool> mGraphicsCommandPool;
    std::unique_ptr<vk2::CommandPool> mTransferCommandPool;

    vk::UniqueRenderPass mRenderPass;
    std::vector<vk::UniqueFramebuffer> mFrameBuffers;

    std::unique_ptr<vk2::CommandBuffers> mCommandBuffers;

    std::unique_ptr<vk2::GraphicsPipeline> mGraphicsPipeline;
    vk2::PipelineStates mPipelineStates;

    std::unique_ptr<vk2::Semaphores> mImageAvailableSemaphores;
    std::unique_ptr<vk2::Semaphores> mRenderFinishedSemaphores;
    std::unique_ptr<vk2::Fences> mFences;

    std::unique_ptr<vk2::Buffer> mGpuVertexBuffer;
    std::unique_ptr<vk2::Buffer> mGpuIndexBuffer;

    std::unique_ptr<vk2::Buffers> mUniformBuffers;
    vk::UniqueDescriptorPool mDescriptorPool;
    MatrixUBO mMatrixUBO;
    vk::UniqueDescriptorSetLayout mDescriptorSetLayout;
    std::vector<vk::DescriptorSet> mDescriptorSets;

    vk::UniqueSampler mTextureSampler;
    std::unique_ptr<vk2::ImageView> mImageView;
};

#endif 