#ifndef APP
#define APP

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "utils/AppInstance.h"
#include "utils/FrameBuffers.h"
#include "utils/GraphicsPipeline.h"
#include "utils/LogicalDevice.h"
#include "utils/PipelineStateFactory.h"
#include "utils/RenderPass.h"
#include "utils/ShaderModule.h"
#include "utils/SwapChain.h"
#include "utils/Window.h"
#include "utils/WindowSurface.h"

class App {
public:
    App(const uint32_t windowWidth,
        const uint32_t windowHeight,
        const char* windowTitle);

    void run();
private:
    vk::RenderPass* createRenderPass() const;

    // The member variables declaration order is important because
    // the destruction order must be this.
    std::unique_ptr<vk::Window> mWindow;
    std::unique_ptr<vk::AppInstance> mAppInstance;
    std::unique_ptr<vk::WindowSurface> mWindowSurface;
    std::unique_ptr<vk::LogicalDevice> mLogicalDevice;
    std::unique_ptr<vk::SwapChain> mSwapChain;
    std::unique_ptr<vk::FrameBuffers> mFrameBuffers; 
    std::unique_ptr<vk::RenderPass> mRenderPass;
    std::unique_ptr<vk::GraphicsPipeline> mGraphicsPipeline;
};

#endif 