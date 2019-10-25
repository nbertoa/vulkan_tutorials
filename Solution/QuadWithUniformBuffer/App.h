#ifndef APP
#define APP

#include "MatrixUBO.h"
#include "Utils/BaseApp.h"
#include "Utils/Buffers.h"
#include "Utils/DescriptorPool.h"

class App : public vk::BaseApp {
public:
    App(const uint32_t windowWidth,
        const uint32_t windowHeight,
        const char* windowTitle,
        const vk::RenderPassCreator& renderPassCreator);

protected:
    void processCurrentFrame() override;

    void createBuffers();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffer();

    void recordCommandBuffers();

    virtual void
    createGraphicsPipeline() override final;

    void
    initPipelineStates(vk::PipelineStates& pipelineStates) const;

    void
    initShaderStages(vk::ShaderStages& shaderStages);

    std::unique_ptr<vk::Buffer> mGpuVertexBuffer;
    std::unique_ptr<vk::Buffer> mGpuIndexBuffer;

    std::unique_ptr<vk::Buffers> mUniformBuffers;
    vk::DescriptorPool mDescriptorPool;


    MatrixUBO mMatrixUBO;
};

#endif 