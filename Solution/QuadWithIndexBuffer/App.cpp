#include "App.h"

#include <cassert>

#include "Utils/CommandPools.h"
#include "Utils/SwapChain.h"
#include "Utils/Window.h"
#include "Utils/device/LogicalDevice.h"
#include "Utils/device/PhysicalDevice.h"
#include "Utils/pipeline/PipelineStates.h"
#include "Utils/shader/ShaderModule.h"
#include "Utils/shader/ShaderModuleSystem.h"
#include "Utils/shader/ShaderStages.h"
#include "Utils/vertex/PosColorVertex.h"

using namespace vulkan;

App::App() {
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
    initVertexBuffer();
    initIndexBuffer();
}

void 
App::initVertexBuffer() {
    assert(mGpuVertexBuffer == nullptr);

    // The vertex position is in screen space.
    std::vector<PosColorVertex> screenSpaceVertices
    {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},          
    };

    const size_t verticesSize = sizeof(PosColorVertex) * screenSpaceVertices.size();

    mGpuVertexBuffer.reset(new Buffer(verticesSize,
                                      vk::BufferUsageFlagBits::eTransferDst |
                                      vk::BufferUsageFlagBits::eVertexBuffer,
                                      vk::MemoryPropertyFlagBits::eDeviceLocal));

    mGpuVertexBuffer->copyFromDataToDeviceMemory(screenSpaceVertices.data(),
                                                 verticesSize);
}

void 
App::initIndexBuffer() {
    assert(mGpuIndexBuffer == nullptr);

    std::vector<uint32_t> indices
    {
        0, 1, 2, // upper-right triangle
        2, 1, 3, // bottom-left triangle
    };

    const size_t indicesSize = sizeof(uint32_t) * indices.size();

    mGpuIndexBuffer.reset(new Buffer(indicesSize,
                                     vk::BufferUsageFlagBits::eTransferDst |
                                     vk::BufferUsageFlagBits::eIndexBuffer,
                                     vk::MemoryPropertyFlagBits::eDeviceLocal));

    mGpuIndexBuffer->copyFromDataToDeviceMemory(indices.data(),
                                                indicesSize);
}

void
App::recordCommandBuffers() {
    assert(mCommandBuffers.empty() == false);
    assert(mFrameBuffers.empty() == false);
    for (size_t i = 0; i < mCommandBuffers.size(); ++i) {
        vk::CommandBuffer& commandBuffer = mCommandBuffers[i].get();

        commandBuffer.begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eSimultaneousUse});

        const vk::ClearColorValue colorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
        const vk::ClearValue clearValue(colorValue);
        vk::RenderPassBeginInfo info;
        info.setRenderArea(vk::Rect2D(vk::Offset2D {0, 0}, mSwapChain.imageExtent()));
        info.setFramebuffer(mFrameBuffers[i].get());
        info.setClearValueCount(1);
        info.setPClearValues(&clearValue);
        info.setRenderPass(mRenderPass.get());
        commandBuffer.beginRenderPass(info,
                                      vk::SubpassContents::eInline);

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                   mGraphicsPipeline->pipeline());

        commandBuffer.bindVertexBuffers(0, // first vertex buffer to bind
                                        {mGpuVertexBuffer->vkBuffer()},
                                        {0}); // offsets \

        commandBuffer.bindIndexBuffer(mGpuIndexBuffer->vkBuffer(),
                                      0, // offset
                                      vk::IndexType::eUint32);

        commandBuffer.drawIndexed(static_cast<uint32_t>(mGpuIndexBuffer->size() / sizeof(uint32_t)),
                                  1,
                                  0,
                                  0,
                                  0);

        commandBuffer.endRenderPass();;

        commandBuffer.end();
    }
}

void 
App::initGraphicsPipeline()  {
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

    vk::RenderPassCreateInfo info;

    vk::AttachmentDescription attachmentDesc;
    attachmentDesc.setFormat(mSwapChain.imageFormat());
    attachmentDesc.setSamples(vk::SampleCountFlagBits::e1);
    attachmentDesc.setLoadOp(vk::AttachmentLoadOp::eClear);
    attachmentDesc.setStoreOp(vk::AttachmentStoreOp::eStore);
    attachmentDesc.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    attachmentDesc.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    attachmentDesc.setInitialLayout(vk::ImageLayout::eUndefined);
    attachmentDesc.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    info.setAttachmentCount(1);
    info.setPAttachments(&attachmentDesc);
    
    vk::AttachmentReference colorAttachment;
    colorAttachment.setAttachment(0);
    colorAttachment.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpassDescription;
    subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpassDescription.setColorAttachmentCount(1);
    subpassDescription.setPColorAttachments(&colorAttachment);
    info.setSubpassCount(1);
    info.setPSubpasses(&subpassDescription);

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
    info.setDependencyCount(1);
    info.setPDependencies(&subpassDependency);

    mRenderPass = LogicalDevice::device().createRenderPassUnique(info);
}

void
App::submitCommandBufferAndPresent() {
    assert(mCommandBuffers.empty() == false);

    const vk::Fence& fence = mFences->nextAvailableFence();
    vk::Device device(LogicalDevice::device());
    device.waitForFences({fence},
                         VK_TRUE,
                         std::numeric_limits<uint64_t>::max());
    device.resetFences({fence});

    vk::Semaphore imageAvailableSemaphore = mImageAvailableSemaphores->nextAvailableSemaphore();
    vk::Semaphore renderFinishedSemaphore = mRenderFinishedSemaphores->nextAvailableSemaphore();

    const uint32_t swapChainImageIndex = mSwapChain.acquireNextImage(imageAvailableSemaphore);
    assert(swapChainImageIndex < mCommandBuffers.size());

    vk::CommandBuffer& commandBuffer = mCommandBuffers[swapChainImageIndex].get();
    vk::SubmitInfo info;
    info.setWaitSemaphoreCount(1);
    info.setPWaitSemaphores(&imageAvailableSemaphore);
    info.setSignalSemaphoreCount(1);
    info.setPSignalSemaphores(&renderFinishedSemaphore);
    info.setCommandBufferCount(1);
    info.setPCommandBuffers(&commandBuffer);
    const vk::PipelineStageFlags flags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    info.setPWaitDstStageMask(&flags);
    LogicalDevice::graphicsQueue().submit({info},
                                          fence);

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
    assert(mCommandBuffers.empty());
    assert(mFrameBuffers.empty() == false);

    vk::CommandBufferAllocateInfo info;
    info.setCommandBufferCount(static_cast<uint32_t>(mFrameBuffers.size()));
    info.setLevel(vk::CommandBufferLevel::ePrimary);
    info.setCommandPool(CommandPools::graphicsCommandPool());
    mCommandBuffers = LogicalDevice::device().allocateCommandBuffersUnique(info);
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
        ShaderModuleSystem::getOrLoadShaderModule("../../QuadWithIndexBuffer/resources/shaders/vert.spv",
                                                  vk::ShaderStageFlagBits::eVertex)
    );
    shaderStages.addShaderModule(
        ShaderModuleSystem::getOrLoadShaderModule("../../QuadWithIndexBuffer/resources/shaders/frag.spv",
                                                  vk::ShaderStageFlagBits::eFragment)
    );
}
