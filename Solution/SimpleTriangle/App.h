#ifndef APP
#define APP

#include "Utils/BaseApp.h"
#include "Utils/Buffer.h"

class App : public vk::BaseApp {
public:
    App(const uint32_t windowWidth,
        const uint32_t windowHeight,
        const char* windowTitle,
        const vk::RenderPassCreator& renderPassCreator);

protected:
    void createBuffers();
    void recordCommandBuffers();

    virtual void
    createGraphicsPipeline() override final;

    void
    initPipelineStates(vk::PipelineStates& pipelineStates) const;

    void
    initShaderStages(vk::ShaderStages& shaderStages);

    std::unique_ptr<vk::Buffer> mGpuVertexBuffer;
};

#endif 