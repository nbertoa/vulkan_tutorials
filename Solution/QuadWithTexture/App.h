#ifndef APP
#define APP

#include "MatrixUBO.h"

#include "Utils/FrameBuffers.h"
#include "Utils/SystemManager.h"
#include "Utils/command/CommandBuffers.h"
#include "Utils/descriptor/DescriptorPool.h"
#include "Utils/descriptor/DescriptorSetLayout.h"
#include "Utils/descriptor/DescriptorSets.h"
#include "Utils/pipeline/GraphicsPipeline.h"
#include "Utils/pipeline_stage/PipelineStates.h"
#include "Utils/render_pass/RenderPass.h"
#include "Utils/resource/Buffers.h"
#include "Utils/resource/ImageView.h"
#include "Utils/resource/Sampler.h"
#include "Utils/sync/Fences.h"
#include "Utils/sync/Semaphores.h"

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
    processCurrentFrame();

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
    initPipelineStates(vk::PipelineStates& pipelineStates) const;

    void
    initShaderStages(vk::ShaderStages& shaderStages);

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
    std::unique_ptr<vk::Buffer> mGpuIndexBuffer;

    std::unique_ptr<vk::Buffers> mUniformBuffers;
    std::unique_ptr<vk::DescriptorPool> mDescriptorPool;

    MatrixUBO mMatrixUBO;
    std::unique_ptr<vk::DescriptorSetLayout> mDescriptorSetLayout;
    std::unique_ptr<vk::DescriptorSets> mDescriptorSets;

    vk::Sampler mTextureSampler;
    std::unique_ptr<vk::ImageView> mImageView;
};

#endif 