#include "App.h"

#include <cassert>

#include "Utils/SwapChain.h"
#include "Utils/Window.h"
#include "Utils/device/LogicalDevice.h"
#include "Utils/device/PhysicalDevice.h"
#include "Utils/pipeline_stage/PipelineStates.h"
#include "Utils/shader/ShaderModule.h"
#include "Utils/shader/ShaderModuleSystem.h"
#include "Utils/shader/ShaderStages.h"
#include "Utils/vertex/PosColorVertex.h"

using namespace vulkan;

App::App() {
    // Init command pools
    vk::CommandPoolCreateInfo info;
    info.setQueueFamilyIndex(PhysicalDevice::graphicsQueueFamilyIndex());
    mGraphicsCommandPool = LogicalDevice::device().createCommandPoolUnique(info);

    info.setQueueFamilyIndex(PhysicalDevice::transferQueueFamilyIndex());
    info.setFlags(vk::CommandPoolCreateFlagBits::eTransient);
    mTransferCommandPool = LogicalDevice::device().createCommandPoolUnique(info);

    initRenderPass();
    initFrameBuffers();
    initCommandBuffers();
    initSemaphoresAndFences();
    initGraphicsPipeline();
    initBuffers();
    recordCommandBuffers();
}

void
App::run() {
    while (Window::shouldCloseWindow() == false) {
        glfwPollEvents();

        submitCommandBufferAndPresent();
    }

    // We need to wait on the hsot for the completion of outstanding
    // operations for a given queue.
    // vkDeviceWaitIdle is equivalent to submitting fences to all
    // the queues owned yb the device a and waiting with an infinite 
    // timeout for these fences to signal.
    LogicalDevice::device().waitIdle();
}

void 
App::initBuffers() {
    assert(mGpuVertexBuffer == nullptr);

    // The vertex position is in screen space.
    std::vector<vulkan::PosColorVertex> screenSpaceVertices
    {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},        
    };

    const uint32_t verticesSize = static_cast<uint32_t>(sizeof(PosColorVertex) * screenSpaceVertices.size());

    mGpuVertexBuffer.reset(new Buffer(verticesSize,
                                      vk::BufferUsageFlagBits::eTransferDst |
                                      vk::BufferUsageFlagBits::eVertexBuffer,
                                      vk::MemoryPropertyFlagBits::eDeviceLocal));

    mGpuVertexBuffer->copyFromDataToDeviceMemory(screenSpaceVertices.data(),
                                                 verticesSize,
                                                 mTransferCommandPool.get());
}

void
App::recordCommandBuffers() {
    assert(mCommandBuffers != nullptr);
    assert(mFrameBuffers.empty() == false);
    for (uint32_t i = 0; i < mCommandBuffers->bufferCount(); ++i) {
        CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(i);

        commandBuffer.beginRecording(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

        commandBuffer.beginPass(*mRenderPass,
                                mFrameBuffers[i].get(),
                                mSwapChain.imageExtent());

        commandBuffer.bindPipeline(*mGraphicsPipeline);

        commandBuffer.bindVertexBuffer(*mGpuVertexBuffer);

        commandBuffer.draw(static_cast<uint32_t>(mGpuVertexBuffer->size() / sizeof(PosColorVertex)));

        commandBuffer.endPass();

        commandBuffer.endRecording();
    }
}

void
App::initGraphicsPipeline() {
    assert(mGraphicsPipeline == nullptr);

    PipelineStates pipelineStates;
    initPipelineStates(pipelineStates);

    ShaderStages shaderStages;
    initShaderStages(shaderStages);

    vk::UniquePipelineLayout pipelineLayout =
        LogicalDevice::device().createPipelineLayoutUnique({});

    mGraphicsPipeline.reset(new GraphicsPipeline(pipelineLayout,
                                                 pipelineStates,
                                                 shaderStages,
                                                 mRenderPass.get()));
}

void
App::initRenderPass() {
    assert(mRenderPass.get() == VK_NULL_HANDLE);

    std::vector<vk::AttachmentDescription> attachmentDescriptions;

    // The format of the color attachment should match the format 
    // of the swap chain images.
    //
    // No multisampling
    //
    // Using vk::ImageLayout::eUndefined for initial layout means
    // that we do not care what previous layout the image was in.
    // We want the image to be ready for presentation using the swap chain 
    // after rendering, which is why we use ImageLayout::PresentSrcKHR
    // for the final layout.
    attachmentDescriptions.emplace_back(vk::AttachmentDescription 
    {
        vk::AttachmentDescriptionFlags(),
        mSwapChain.imageFormat(),
        vk::SampleCountFlagBits::e1, // sample count
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare, // depth/stencil buffer
        vk::AttachmentStoreOp::eDontCare, // depth/stencil buffer
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR
    });

    const std::vector<vk::AttachmentReference> colorAttachments {
        {0, vk::ImageLayout::eColorAttachmentOptimal}
    };
    std::vector<vk::SubpassDescription> subpassDescriptions;
    subpassDescriptions.emplace_back(vk::SubpassDescription
    {
        vk::SubpassDescriptionFlags(),
        vk::PipelineBindPoint::eGraphics,
        0, // input attachment count
        nullptr, // input attachments
        static_cast<uint32_t>(colorAttachments.size()),
        colorAttachments.data(),
    });

    // VK_SUBPASS_EXTERNAL: Implicit subpass before or after the render pass 
    // depending on whether it is specified in srcSubpass or dstSubpass.
    //
    // Specify the operations to wait on and the stages in which these operations occur.
    // We need to wait for the swap chain to finish reading from the image
    // before we can access it.
    // This can be accomplished by waiting on the color attachment output stage itself.
    //
    // The operations that should wait on this are in the color attachment stage
    // adn involve the reading and writing of the color attachment.
    // These settings will prevent the transition from happening until it is
    // actually necessary (and allowed): when we want to start writing colors
    // to it.
    vk::SubpassDependency subpassDependency;
    subpassDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
    subpassDependency.setDstSubpass(0);
    subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    subpassDependency.setSrcAccessMask(vk::AccessFlags());
    subpassDependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                                       vk::AccessFlagBits::eColorAttachmentWrite);
    std::vector<vk::SubpassDependency> subpassDependencies {subpassDependency};

    vk::RenderPassCreateInfo info;
    info.setAttachmentCount(static_cast<uint32_t>(attachmentDescriptions.size()));
    info.setPAttachments(attachmentDescriptions.empty() ? nullptr : attachmentDescriptions.data());
    info.setSubpassCount(static_cast<uint32_t>(subpassDescriptions.size()));
    info.setPSubpasses(subpassDescriptions.empty() ? nullptr : subpassDescriptions.data());
    info.setDependencyCount(static_cast<uint32_t>(subpassDependencies.size()));
    info.setPDependencies(subpassDependencies.empty() ? nullptr : subpassDependencies.data());
    mRenderPass = LogicalDevice::device().createRenderPassUnique(info);
}

void
App::submitCommandBufferAndPresent() {
    assert(mCommandBuffers != nullptr);

    const vk::Fence& fence = mFences->nextAvailableFence();
    vk::Device device(LogicalDevice::device());
    device.waitForFences({fence},
                         VK_TRUE,
                         std::numeric_limits<uint64_t>::max());
    device.resetFences({fence});

    vk::Semaphore imageAvailableSemaphore = mImageAvailableSemaphores->nextAvailableSemaphore();
    vk::Semaphore renderFinishedSemaphore = mRenderFinishedSemaphores->nextAvailableSemaphore();

    const uint32_t swapChainImageIndex = mSwapChain.acquireNextImage(imageAvailableSemaphore);
    assert(swapChainImageIndex < mCommandBuffers->bufferCount());

    CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(swapChainImageIndex);
    commandBuffer.submit(LogicalDevice::graphicsQueue(),
                         &imageAvailableSemaphore,
                         &renderFinishedSemaphore,
                         fence,
                         vk::PipelineStageFlagBits::eColorAttachmentOutput);

    mSwapChain.present(renderFinishedSemaphore,
                       swapChainImageIndex);
}

void
App::initFrameBuffers() {
    assert(mFrameBuffers.empty());
    assert(mRenderPass.get() != VK_NULL_HANDLE);
    assert(mSwapChain.imageViews().empty() == false);

    vk::FramebufferCreateInfo info;
    info.setRenderPass(mRenderPass.get());
    info.setAttachmentCount(1);
    info.setWidth(mSwapChain.imageWidth());
    info.setHeight(mSwapChain.imageHeight());
    info.setLayers(1);

    const std::vector<vk::UniqueImageView>& imageViews = mSwapChain.imageViews();
    mFrameBuffers.resize(imageViews.size());
    for (size_t i = 0; i < imageViews.size(); ++i) {
        assert(imageViews[i].get() != VK_NULL_HANDLE);
        const vk::ImageView attachments[] = {imageViews[i].get()};
        info.setPAttachments(&imageViews[i].get());

        mFrameBuffers[i] = LogicalDevice::device().createFramebufferUnique(info);
    }
}

void
App::initCommandBuffers() {
    assert(mCommandBuffers == nullptr);
    assert(mFrameBuffers.empty() == false);

    mCommandBuffers.reset(new CommandBuffers(mGraphicsCommandPool.get(),
                                             mFrameBuffers.size(),
                                             vk::CommandBufferLevel::ePrimary));
}

void
App::initSemaphoresAndFences() {
    assert(mImageAvailableSemaphores == nullptr);
    assert(mRenderFinishedSemaphores == nullptr);
    assert(mFences == nullptr);

    mImageAvailableSemaphores.reset(new Semaphores(mFrameBuffers.size()));
    mRenderFinishedSemaphores.reset(new Semaphores(mFrameBuffers.size()));
    mFences.reset(new Fences(mFrameBuffers.size()));
}

void
App::initPipelineStates(PipelineStates& pipelineStates) const {
    std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions;
    PosColorVertex::vertexInputBindingDescriptions(vertexInputBindingDescriptions);

    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;
    PosColorVertex::vertexInputAttributeDescriptions(vertexInputAttributeDescriptions);

    pipelineStates.setVertexInputState({vertexInputBindingDescriptions,
                                       vertexInputAttributeDescriptions});

    pipelineStates.setInputAssemblyState({vk::PrimitiveTopology::eTriangleList,
                                          VK_FALSE});

    pipelineStates.setViewportState({mSwapChain.viewport(),
                                     mSwapChain.scissorRect()});

    pipelineStates.setRasterizationState({});

    pipelineStates.setMultisampleState({});

    const ColorBlendAttachmentState colorBlendAttachmentState;
    pipelineStates.setColorBlendState({colorBlendAttachmentState});
}

void
App::initShaderStages(ShaderStages& shaderStages) {
    shaderStages.addShaderModule(
        ShaderModuleSystem::getOrLoadShaderModule("../../SimpleTriangle/resources/shaders/vert.spv",
                                                  vk::ShaderStageFlagBits::eVertex)
    );
    shaderStages.addShaderModule(
        ShaderModuleSystem::getOrLoadShaderModule("../../SimpleTriangle/resources/shaders/frag.spv",
                                                  vk::ShaderStageFlagBits::eFragment)
    );
}