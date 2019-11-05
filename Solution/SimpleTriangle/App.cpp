#include "App.h"

#include <cassert>

#include "Utils/DebugUtils.h"
#include "Utils/device/LogicalDevice.h"
#include "Utils/pipeline/PipelineLayout.h"
#include "Utils/pipeline_stage/PipelineStates.h"
#include "Utils/render_pass/RenderPass.h"
#include "Utils/shader/ShaderModule.h"
#include "Utils/shader/ShaderStages.h"
#include "Utils/vertex/PosColorVertex.h"

using namespace vk;

App::App(const uint32_t windowWidth,
         const uint32_t windowHeight,
         const char* windowTitle)
    : mSystemManager(windowWidth,
                     windowHeight,
                     windowTitle)
{
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
    while (mSystemManager.window().shouldCloseWindow() == false) {
        glfwPollEvents();

        submitCommandBufferAndPresent();
    }

    // We need to wait on the hsot for the completion of outstanding
    // operations for a given queue.
    // vkDeviceWaitIdle is equivalent to submitting fences to all
    // the queues owned yb the device a and waiting with an infinite 
    // timeout for these fences to signal.
    vkChecker(vkDeviceWaitIdle(mSystemManager.logicalDevice().vkDevice()));
}

void 
App::initBuffers() {
    assert(mGpuVertexBuffer == nullptr);

    // The vertex position is in screen space.
    std::vector<vk::PosColorVertex> vertices
    {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        
    };

    const uint32_t verticesSize = static_cast<uint32_t>(sizeof(PosColorVertex) * vertices.size());

    Buffer cpuVertexBuffer(mSystemManager.logicalDevice(),
                           mSystemManager.physicalDevice(),
                           verticesSize,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           VK_SHARING_MODE_EXCLUSIVE);

    cpuVertexBuffer.copyToHostMemory(vertices.data(),
                                     verticesSize,
                                     0);

    mGpuVertexBuffer.reset(new Buffer(mSystemManager.logicalDevice(),
                                      mSystemManager.physicalDevice(),
                                      verticesSize,
                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,                                      
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                      VK_SHARING_MODE_EXCLUSIVE));

    mGpuVertexBuffer->copyFromBufferToDeviceMemory(cpuVertexBuffer,
                                                   mSystemManager.transferCommandPool());
}

void
App::recordCommandBuffers() {
    assert(mCommandBuffers != nullptr);
    assert(mFrameBuffers != nullptr);
    for (uint32_t i = 0; i < mCommandBuffers->bufferCount(); ++i) {
        CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(i);

        commandBuffer.beginRecording(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

        commandBuffer.beginPass(*mRenderPass,
                                mFrameBuffers->buffer(i),
                                mSystemManager.swapChain().imageExtent());

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

    PipelineLayout pipelineLayout(mSystemManager.logicalDevice());

    mGraphicsPipeline.reset(new GraphicsPipeline(mSystemManager.logicalDevice(),
                                                 *mRenderPass,
                                                 0,
                                                 pipelineLayout,
                                                 pipelineStates,
                                                 shaderStages));
}

void
App::initRenderPass() {
    assert(mRenderPass == nullptr);

    std::vector<AttachmentDescription> attachmentDescriptions;

    // The format of the color attachment should match the format 
    // of the swap chain images.
    //
    // No multisampling
    //
    // Using VK_IMAGE_LAYOUT_UNDEFINED for initial layout means
    // that we do not care what previous layout the image was in.
    // We want the image to be ready for presentation using the swap chain 
    // after rendering, which is why we use VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    // for the final layout.
    attachmentDescriptions.emplace_back(mSystemManager.swapChain().imageFormat(),
                                        VK_ATTACHMENT_LOAD_OP_CLEAR,
                                        VK_ATTACHMENT_STORE_OP_STORE,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    std::vector<VkAttachmentReference> colorAttachmentReferences {{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};
    std::vector<SubpassDescription> subpassDescriptions;
    subpassDescriptions.emplace_back(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                     colorAttachmentReferences);

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
    std::vector<SubpassDependency> subpassDependencies;
    subpassDependencies.emplace_back(VK_SUBPASS_EXTERNAL,
                                     0,
                                     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                     0,
                                     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                     VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                     VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    mRenderPass.reset(new RenderPass(mSystemManager.logicalDevice(),
                                     attachmentDescriptions,
                                     subpassDescriptions,
                                     subpassDependencies));
}

void
App::submitCommandBufferAndPresent() {
    assert(mCommandBuffers != nullptr);

    const Fence& fence = mFences->nextAvailableFence();
    fence.waitAndReset();

    Semaphore& imageAvailableSemaphore = mImageAvailableSemaphores->nextAvailableSemaphore();
    Semaphore& renderFinishedSemaphore = mRenderFinishedSemaphores->nextAvailableSemaphore();

    const uint32_t swapChainImageIndex = mSystemManager.swapChain().acquireNextImage(imageAvailableSemaphore);
    assert(swapChainImageIndex < mCommandBuffers->bufferCount());

    CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(swapChainImageIndex);
    commandBuffer.submit(mSystemManager.logicalDevice().graphicsQueue(),
                         &imageAvailableSemaphore,
                         &renderFinishedSemaphore,
                         fence,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    mSystemManager.swapChain().present(renderFinishedSemaphore,
                                       swapChainImageIndex);
}

void
App::initFrameBuffers() {
    assert(mFrameBuffers == nullptr);
    assert(mRenderPass != nullptr);

    mFrameBuffers.reset(new FrameBuffers(mSystemManager.logicalDevice(),
                                         *mRenderPass,
                                         mSystemManager.swapChain().imageViews(),
                                         mSystemManager.swapChain().imageWidth(),
                                         mSystemManager.swapChain().imageHeight()));
}

void
App::initCommandBuffers() {
    assert(mCommandBuffers == nullptr);
    assert(mFrameBuffers != nullptr);

    mCommandBuffers.reset(new CommandBuffers(mSystemManager.logicalDevice(),
                                             mSystemManager.graphicsCommandPool(),
                                             mFrameBuffers->bufferCount(),
                                             VK_COMMAND_BUFFER_LEVEL_PRIMARY));
}

void
App::initSemaphoresAndFences() {
    assert(mImageAvailableSemaphores == nullptr);
    assert(mRenderFinishedSemaphores == nullptr);
    assert(mFences == nullptr);

    mImageAvailableSemaphores.reset(new Semaphores(mSystemManager.logicalDevice(),
                                                   mFrameBuffers->bufferCount()));
    mRenderFinishedSemaphores.reset(new Semaphores(mSystemManager.logicalDevice(),
                                                   mFrameBuffers->bufferCount()));
    mFences.reset(new Fences(mSystemManager.logicalDevice(),
                             mFrameBuffers->bufferCount()));
}

void
App::initPipelineStates(PipelineStates& pipelineStates) const {
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
    PosColorVertex::vertexInputBindingDescriptions(vertexInputBindingDescriptions);

    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
    PosColorVertex::vertexInputAttributeDescriptions(vertexInputAttributeDescriptions);

    pipelineStates.setVertexInputState({vertexInputBindingDescriptions,
                                       vertexInputAttributeDescriptions});

    pipelineStates.setInputAssemblyState({VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                         VK_FALSE});

    pipelineStates.setViewportState({mSystemManager.swapChain().viewport(),
                                    mSystemManager.swapChain().scissorRect()});

    pipelineStates.setRasterizationState({});

    pipelineStates.setMultisampleState({});

    const ColorBlendAttachmentState colorBlendAttachmentState;
    pipelineStates.setColorBlendState({colorBlendAttachmentState});
}

void
App::initShaderStages(ShaderStages& shaderStages) {
    ShaderModuleSystem& shaderModuleSystem = mSystemManager.shaderModuleSystem();
    shaderStages.addShaderModule(shaderModuleSystem.getOrLoadShaderModule("../../SimpleTriangle/resources/shaders/vert.spv",
                                                                          VK_SHADER_STAGE_VERTEX_BIT));
    shaderStages.addShaderModule(shaderModuleSystem.getOrLoadShaderModule("../../SimpleTriangle/resources/shaders/frag.spv",
                                                                          VK_SHADER_STAGE_FRAGMENT_BIT));
}