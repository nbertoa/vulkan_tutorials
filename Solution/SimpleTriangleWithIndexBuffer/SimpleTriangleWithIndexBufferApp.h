#ifndef SIMPLE_TRIANGLE_WITH_INDEX_BUFFER_APP
#define SIMPLE_TRIANGLE_WITH_INDEX_BUFFER_APP

#include "Utils/BaseApp.h"
#include "Utils/Buffer.h"

class SimpleTriangleWithIndexBufferApp : public vk::BaseApp {
public:
    SimpleTriangleWithIndexBufferApp(const uint32_t windowWidth,
                                     const uint32_t windowHeight,
                                     const char* windowTitle,
                                     const vk::RenderPassCreator& renderPassCreator,
                                     const vk::GraphicsPipelineCreator& graphicsPipelineCreator);

protected:
    void createBuffers();
    void createVertexBuffer();
    void createIndexBuffer();

    void recordCommandBuffers();

    std::unique_ptr<vk::Buffer> mGpuVertexBuffer;
    std::unique_ptr<vk::Buffer> mGpuIndexBuffer;
};

#endif 