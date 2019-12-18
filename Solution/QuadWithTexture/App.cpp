#include "App.h"

#include <cassert>

#include "Utils/SwapChain.h"
#include "Utils/Window.h"
#include "Utils/device/LogicalDevice.h"
#include "Utils/device/PhysicalDevice.h"
#include "Utils/pipeline/PipelineStates.h"
#include "Utils/resource/Image.h"
#include "Utils/resource/ImageSystem.h"
#include "Utils/shader/ShaderModule.h"
#include "Utils/shader/ShaderModuleSystem.h"
#include "Utils/shader/ShaderStages.h"
#include "Utils/vertex/PosTexCoordVertex.h"

using namespace vulkan;

App::App() {
    // Init command pools
    vk::CommandPoolCreateInfo info;
    info.setQueueFamilyIndex(PhysicalDevice::graphicsQueueFamilyIndex());
    mGraphicsCommandPool = LogicalDevice::device().createCommandPoolUnique(info);

    info.setQueueFamilyIndex(PhysicalDevice::transferQueueFamilyIndex());
    info.setFlags(vk::CommandPoolCreateFlagBits::eTransient);
    mTransferCommandPool = LogicalDevice::device().createCommandPoolUnique(info);

    initUniformBuffers();
    initVertexBuffer();
    initIndexBuffer();
    initImages();
    initDescriptorSets();
    initRenderPass();
    initFrameBuffers();
    initCommandBuffers();
    initSemaphoresAndFences();
    initGraphicsPipeline();    
    recordCommandBuffers();
}

void
App::run() {
    while (Window::shouldCloseWindow() == false) {
        glfwPollEvents();

        vk::Semaphore imageAvailableSemaphore = mImageAvailableSemaphores->nextAvailableSemaphore();
        mSwapChain.acquireNextImage(imageAvailableSemaphore);

        updateUniformBuffers();

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
App::updateUniformBuffers() {
    // Update uniform buffers
    const uint32_t currentSwapChainImageIndex = mSwapChain.currentImageIndex();
    mMatrixUBO.update(currentSwapChainImageIndex,
                      mSwapChain.imageAspectRatio());
    Buffer& uniformBuffer = mUniformBuffers[currentSwapChainImageIndex];
    uniformBuffer.copyToHostMemory(&mMatrixUBO);
}

void
App::initDescriptorSets() {
    assert(mDescriptorPool.get() == VK_NULL_HANDLE);

    const uint32_t imageViewCount = mSwapChain.imageViewCount();

    vk::DescriptorPoolSize descPoolSizes[2];
    descPoolSizes[0].setDescriptorCount(imageViewCount);
    descPoolSizes[0].setType(vk::DescriptorType::eUniformBuffer);
    descPoolSizes[1].setDescriptorCount(imageViewCount);
    descPoolSizes[1].setType(vk::DescriptorType::eCombinedImageSampler);

    vk::DescriptorPoolCreateInfo descPoolInfo;
    descPoolInfo.setMaxSets(imageViewCount);
    descPoolInfo.setPoolSizeCount(2);
    descPoolInfo.setPPoolSizes(descPoolSizes);

    mDescriptorPool = LogicalDevice::device().createDescriptorPoolUnique(descPoolInfo);

    assert(mDescriptorSetLayout.get() == VK_NULL_HANDLE);

    vk::DescriptorSetLayoutBinding descSetLayoutBinding[2];
    descSetLayoutBinding[0].setBinding(0);
    descSetLayoutBinding[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
    descSetLayoutBinding[0].setDescriptorCount(1);
    descSetLayoutBinding[0].setStageFlags(vk::ShaderStageFlagBits::eVertex);
    descSetLayoutBinding[1].setBinding(1);
    descSetLayoutBinding[1].setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    descSetLayoutBinding[1].setDescriptorCount(1);
    descSetLayoutBinding[1].setStageFlags(vk::ShaderStageFlagBits::eFragment);

    vk::DescriptorSetLayoutCreateInfo descSetLayoutInfo;
    descSetLayoutInfo.setBindingCount(2);
    descSetLayoutInfo.setPBindings(descSetLayoutBinding);

    mDescriptorSetLayout =
        LogicalDevice::device().createDescriptorSetLayoutUnique(descSetLayoutInfo);

    const std::vector<vk::DescriptorSetLayout> descSetLayouts(imageViewCount,
                                                              mDescriptorSetLayout.get());

    vk::DescriptorSetAllocateInfo allocateInfo;
    allocateInfo.setDescriptorPool(mDescriptorPool.get());
    allocateInfo.setDescriptorSetCount(imageViewCount);
    allocateInfo.setPSetLayouts(descSetLayouts.data());

    // Create a descriptor set for each swap chain image, all with the same layout.
    mDescriptorSets = LogicalDevice::device().allocateDescriptorSets(allocateInfo);

    // The descriptor sets have been allocated now, but the descriptors within still
    // need to be configured.
    vk::DescriptorBufferInfo bufferInfo;
    bufferInfo.setRange(sizeof(MatrixUBO));

    assert(mImageView.get() != VK_NULL_HANDLE);
    vk::DescriptorImageInfo imageInfo;
    imageInfo.setImageView(mImageView.get());
    imageInfo.setSampler(mTextureSampler.get());
    imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
                                                  
    for (uint32_t i = 0; i < imageViewCount; ++i) {
        bufferInfo.setBuffer(mUniformBuffers[i].vkBuffer());

        vk::WriteDescriptorSet bufferWrite;
        bufferWrite.setDescriptorCount(1);
        bufferWrite.setDstSet(mDescriptorSets[i]);
        bufferWrite.setDescriptorType(vk::DescriptorType::eUniformBuffer);
        bufferWrite.setPBufferInfo(&bufferInfo);
        bufferWrite.setDstBinding(0);

        vk::WriteDescriptorSet imageWrite;
        imageWrite.setDescriptorCount(1);
        imageWrite.setDstSet(mDescriptorSets[i]);
        imageWrite.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
        imageWrite.setPImageInfo(&imageInfo);
        imageWrite.setDstBinding(1);

        LogicalDevice::device().updateDescriptorSets({bufferWrite,
                                                      imageWrite},
                                                     {});
    }
}

void
App::initImages() {
    assert(mImageView.get() == VK_NULL_HANDLE);
    assert(mTextureSampler.get() == VK_NULL_HANDLE);

    mTextureSampler = LogicalDevice::device().createSamplerUnique({});

    const std::string path = "../../../external/resources/textures/flowers/dahlia.jpg";
    Image& image = ImageSystem::getOrLoadImage(path,
                                               *mTransferCommandPool);

    image.transitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal,
                                *mTransferCommandPool);

    vk::ImageViewCreateInfo info;
    info.setImage(image.vkImage());
    info.setFormat(vk::Format::eR8G8B8A8Unorm);
    info.setSubresourceRange(vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    info.setViewType(vk::ImageViewType::e2D);
    mImageView = LogicalDevice::device().createImageViewUnique(info);
}

void 
App::initVertexBuffer() {
    assert(mGpuVertexBuffer == nullptr);

    std::vector<PosTexCoordVertex> screenSpaceVertices
    {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}}
    };

    const size_t verticesSize = sizeof(PosTexCoordVertex) * screenSpaceVertices.size();

    mGpuVertexBuffer.reset(new Buffer(verticesSize,
                                      vk::BufferUsageFlagBits::eTransferDst |
                                      vk::BufferUsageFlagBits::eVertexBuffer,
                                      vk::MemoryPropertyFlagBits::eDeviceLocal));

    mGpuVertexBuffer->copyFromDataToDeviceMemory(screenSpaceVertices.data(),
                                                 verticesSize,
                                                 *mTransferCommandPool);
}

void 
App::initIndexBuffer() {
    assert(mGpuIndexBuffer == nullptr);

    std::vector<uint32_t> indices
    {
        0, 1, 2, // upper-right triangle
        2, 3, 0, // bottom-left triangle
    };

    const size_t indicesSize = sizeof(uint32_t) * indices.size();

    mGpuIndexBuffer.reset(new Buffer(indicesSize,
                                     vk::BufferUsageFlagBits::eTransferDst |
                                     vk::BufferUsageFlagBits::eIndexBuffer,
                                     vk::MemoryPropertyFlagBits::eDeviceLocal));

    mGpuIndexBuffer->copyFromDataToDeviceMemory(indices.data(),
                                                indicesSize,
                                                *mTransferCommandPool);
}

void
App::initUniformBuffers() {
    assert(mUniformBuffers.empty());

    for (uint32_t i = 0; i < mSwapChain.imageViewCount(); ++i) {
        Buffer buffer(sizeof(MatrixUBO),
                      vk::BufferUsageFlagBits::eUniformBuffer,
                      vk::MemoryPropertyFlagBits::eHostVisible |
                      vk::MemoryPropertyFlagBits::eHostCoherent);

        mUniformBuffers.emplace_back(std::move(buffer));
    }
}

void
App::recordCommandBuffers() {
    assert(mCommandBuffers.empty() == false);
    assert(mFrameBuffers.empty() == false);
    for (uint32_t i = 0; i < mCommandBuffers.size(); ++i) {
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
                                        {0}); // offsets 

        commandBuffer.bindIndexBuffer(mGpuIndexBuffer->vkBuffer(),
                                      0, // offset
                                      vk::IndexType::eUint32);

        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                         mGraphicsPipeline->pipelineLayout(),
                                         0, // first descriptor set
                                         {mDescriptorSets[i]},
                                         {}); // dynamic arrays

        commandBuffer.drawIndexed(static_cast<uint32_t>(mGpuIndexBuffer->size() / sizeof(uint32_t)),
                                  1,
                                  0,
                                  0,
                                  0);

        commandBuffer.endRenderPass();

        commandBuffer.end();
    }
}

void
App::initGraphicsPipeline() {
    assert(mGraphicsPipeline == nullptr);
    assert(mDescriptorSetLayout.get() != VK_NULL_HANDLE);

    PipelineStates pipelineStates;
    initPipelineStates(pipelineStates);

    ShaderStages shaderStages;
    initShaderStages(shaderStages);

    vk::PipelineLayoutCreateInfo info;
    info.setSetLayoutCount(1);
    info.setPSetLayouts(&mDescriptorSetLayout.get());

    vk::UniquePipelineLayout pipelineLayout =
        LogicalDevice::device().createPipelineLayoutUnique(info);

    mGraphicsPipeline.reset(new GraphicsPipeline(pipelineLayout,
                                                 pipelineStates,
                                                 shaderStages,
                                                 mRenderPass.get()));
}

void
App::initPipelineStates(PipelineStates& pipelineStates) const {
    std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions;
    PosTexCoordVertex::vertexInputBindingDescriptions(vertexInputBindingDescriptions);

    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;
    PosTexCoordVertex::vertexInputAttributeDescriptions(vertexInputAttributeDescriptions);

    pipelineStates.setVertexInputState({vertexInputBindingDescriptions,
                                        vertexInputAttributeDescriptions});

    pipelineStates.setInputAssemblyState({vk::PrimitiveTopology::eTriangleList,
                                         VK_FALSE});

    pipelineStates.setViewportState({mSwapChain.viewport(),
                                     mSwapChain.scissorRect()});

    pipelineStates.setRasterizationState(RasterizationState());

    pipelineStates.setMultisampleState({});

    const ColorBlendAttachmentState colorBlendAttachmentState;
    pipelineStates.setColorBlendState({colorBlendAttachmentState});
}

void
App::initShaderStages(ShaderStages& shaderStages) {
    shaderStages.addShaderModule(
        ShaderModuleSystem::getOrLoadShaderModule("../../QuadWithTexture/resources/shaders/vert.spv",
                                                  vk::ShaderStageFlagBits::eVertex)
    );
    shaderStages.addShaderModule(
        ShaderModuleSystem::getOrLoadShaderModule("../../QuadWithTexture/resources/shaders/frag.spv",
                                                  vk::ShaderStageFlagBits::eFragment)
    );
}

void
App::initRenderPass() {
    assert(mRenderPass.get() == VK_NULL_HANDLE);

    vk::RenderPassCreateInfo info;

    vk::AttachmentDescription attachmentDescription;
    attachmentDescription.setFormat(mSwapChain.imageFormat());
    attachmentDescription.setSamples(vk::SampleCountFlagBits::e1);
    attachmentDescription.setLoadOp(vk::AttachmentLoadOp::eClear);
    attachmentDescription.setStoreOp(vk::AttachmentStoreOp::eStore);
    attachmentDescription.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    attachmentDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    attachmentDescription.setInitialLayout(vk::ImageLayout::eUndefined);
    attachmentDescription.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    info.setAttachmentCount(1);
    info.setPAttachments(&attachmentDescription);

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

    // This semaphore was already obtained in run()
    vk::Semaphore imageAvailableSemaphore = mImageAvailableSemaphores->currentSemaphore();
    vk::Semaphore renderFinishedSemaphore = mRenderFinishedSemaphores->nextAvailableSemaphore();

    // The next image was already obtained in run()
    const uint32_t swapChainImageIndex = mSwapChain.currentImageIndex();
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
    info.setCommandPool(mGraphicsCommandPool.get());
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