#ifndef APP
#define APP

#include "Utils/BaseApp.h"
#include "Utils/Buffer.h"

namespace vk {
class PipelineStates;
class ShaderStages;
}

class App : public vk::BaseApp {
public:
    App(const uint32_t windowWidth,
        const uint32_t windowHeight,
        const char* windowTitle,
        const vk::RenderPassCreator& renderPassCreator);

protected:
    void createBuffers();
    void createVertexBuffer();
    void createIndexBuffer();

    void recordCommandBuffers();

    virtual void 
    createGraphicsPipeline() override final;

    void 
    initPipelineStates(vk::PipelineStates& pipelineStates) const;

    void
    initShaderStages(vk::ShaderStages& shaderStages);

    std::unique_ptr<vk::Buffer> mGpuVertexBuffer;
    std::unique_ptr<vk::Buffer> mGpuIndexBuffer;
};

#endif 