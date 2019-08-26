#ifndef APP
#define APP

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "utils/CommandBuffers.h"
#include "utils/FrameBuffers.h"
#include "utils/GraphicsPipeline.h"
#include "utils/RenderPass.h"
#include "utils/Semaphores.h"
#include "utils/ShaderModule.h"
#include "utils/SystemManager.h"

class App {
public:
    App(const uint32_t windowWidth,
        const uint32_t windowHeight,
        const char* windowTitle);

    void run();
private:
    vk::RenderPass* createRenderPass() const;
    vk::GraphicsPipeline* createPipeline();

    void recordCommandBuffers();

    void submitCommandBufferAndPresent();

    // The member variables declaration order is important because
    // the destruction order must be this.
    vk::SystemManager mSystemManager;     
    std::unique_ptr<vk::RenderPass> mRenderPass;
    std::unique_ptr<vk::GraphicsPipeline> mGraphicsPipeline;
    std::unique_ptr<vk::FrameBuffers> mFrameBuffers;
    std::unique_ptr<vk::CommandBuffers> mCommandBuffers;

    vk::Semaphores mImageAvailableSemaphores;
    vk::Semaphores mRenderFinishedSemaphores;
};

#endif 