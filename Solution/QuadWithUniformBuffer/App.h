#ifndef APP
#define APP

#include "MatrixUBO.h"
#include "Utils/BaseApp.h"
#include "Utils/Buffers.h"

class App : public vk::BaseApp {
public:
    App(const uint32_t windowWidth,
        const uint32_t windowHeight,
        const char* windowTitle,
        const vk::RenderPassCreator& renderPassCreator,
        const vk::GraphicsPipelineCreator& graphicsPipelineCreator);

protected:
    void processCurrentFrame() override;

    void createBuffers();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffer();

    void recordCommandBuffers();

    std::unique_ptr<vk::Buffer> mGpuVertexBuffer;
    std::unique_ptr<vk::Buffer> mGpuIndexBuffer;

    std::unique_ptr<vk::Buffers> mUniformBuffers;

    MatrixUBO mMatrixUBO;
};

#endif 