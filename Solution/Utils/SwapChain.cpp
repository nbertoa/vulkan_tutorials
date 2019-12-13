#include "SwapChain.h"

#include <algorithm>
#include <cassert>

#include "Window.h"
#include "device/LogicalDevice.h"
#include "device/PhysicalDevice.h"

namespace vulkan {
SwapChain::SwapChain() {       
    initSwapChain();
    initImagesAndViews();
    initViewportAndScissorRect();
}

uint32_t 
SwapChain::acquireNextImage(const vk::Semaphore semaphore) {
    assert(semaphore != VK_NULL_HANDLE);
    assert(mSwapChain.get() != VK_NULL_HANDLE);

    LogicalDevice::device().acquireNextImageKHR(mSwapChain.get(),
                                                std::numeric_limits<uint64_t>::max(),
                                                semaphore,
                                                vk::Fence(),
                                                & mCurrentImageIndex);

    return mCurrentImageIndex;
}

uint32_t 
SwapChain::currentImageIndex() const {
    assert(mCurrentImageIndex != std::numeric_limits<uint32_t>::max());
    return mCurrentImageIndex;
}

void
SwapChain::present(const vk::Semaphore waitSemaphore,
                   const uint32_t imageIndex) {
    assert(waitSemaphore != VK_NULL_HANDLE);
    assert(mSwapChain.get() != VK_NULL_HANDLE);

    vk::PresentInfoKHR info;
    info.setSwapchainCount(1);
    info.setPSwapchains(&mSwapChain.get());
    info.setWaitSemaphoreCount(1);
    info.setPWaitSemaphores(&waitSemaphore);
    info.setPImageIndices(&imageIndex);
    LogicalDevice::presentationQueue().presentKHR(info);
}

const vk::Viewport& 
SwapChain::viewport() const {
    assert(mSwapChain.get() != VK_NULL_HANDLE);
    return mViewport;
}

const vk::Rect2D& 
SwapChain::scissorRect() const {
    assert(mSwapChain.get() != VK_NULL_HANDLE);
    return mScissorRect;
}

vk::Format
SwapChain::imageFormat() const {
    assert(mSwapChain.get() != VK_NULL_HANDLE);
    return mImageFormat;
}

uint32_t 
SwapChain::imageViewCount() const {
    assert(mSwapChain.get() != VK_NULL_HANDLE);
    return static_cast<uint32_t>(mSwapChainImageViews.size());
}

const std::vector<vk::UniqueImageView>&
SwapChain::imageViews() const {
    assert(mSwapChain.get() != VK_NULL_HANDLE);
    return mSwapChainImageViews;
}

uint32_t
SwapChain::imageWidth() const {
    assert(mSwapChain.get() != VK_NULL_HANDLE);
    return mExtent.width;
}

uint32_t
SwapChain::imageHeight() const {
    assert(mSwapChain.get() != VK_NULL_HANDLE);
    return mExtent.height;
}

float 
SwapChain::imageAspectRatio() const {
    assert(mSwapChain.get() != VK_NULL_HANDLE);
    return mExtent.width / static_cast<float>(mExtent.height);
}

const vk::Extent2D&
SwapChain::imageExtent() const {
    assert(mSwapChain.get() != VK_NULL_HANDLE);
    return mExtent;
}

vk::SurfaceFormatKHR
SwapChain::bestFitSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats) {
    assert(surfaceFormats.empty() == false);

    for (const vk::SurfaceFormatKHR& surfaceFormat : surfaceFormats) {
        // For the color space we will use SRGB if it is available,
        // because it results in more accurate perceived colors.
        // For the format, we will use the standard RGB.
        if (surfaceFormat.format == vk::Format::eB8G8R8A8Unorm &&
            surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return surfaceFormat;
        }
    }

    return surfaceFormats[0];
}

vk::PresentModeKHR
SwapChain::bestFitPresentMode(const std::vector<vk::PresentModeKHR>& presentModes) {
    // Some drivers currently do not properly support PresentModeKHR::eFifo.
    // So we should prefer PresentModeKHR::eImmediate if PresentModeKHR::eMailbox
    // is not available.
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const vk::PresentModeKHR presentMode : presentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            return presentMode;
        } else if (presentMode == vk::PresentModeKHR::eImmediate) {
            bestMode = vk::PresentModeKHR::eImmediate;
        }
    }

    return bestMode;
}

vk::Extent2D
SwapChain::swapChainExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities, 
                           const uint32_t windowWidth, 
                           const uint32_t windowHeight) {
    // Vulkan tells us to match the resolution of the window by setting the width 
    // and height in the currentExtent member.
    // However, some window managers do allow us to differ here and 
    // this is indicated by setting the width and height in currentExtent to a special value. 
    // It is the maximum value of uint32_t.
    // In that case, we will pick the resolution that best matches the window 
    // within the minImageExtent and maxImageExtent bounds.

    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return surfaceCapabilities.currentExtent;
    } else {
        vk::Extent2D actualExtent = {windowWidth, windowHeight};

        actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width, 
                                      std::min(surfaceCapabilities.maxImageExtent.width, 
                                               actualExtent.width));
        actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height, 
                                       std::min(surfaceCapabilities.maxImageExtent.height, 
                                                actualExtent.height));

        return actualExtent;
    }
}

uint32_t
SwapChain::swapChainImageCount(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities) {
    // As we may sometimes have to wait on the driver to complete internal operations
    // before we can acquire another image to render to, then we request at least
    // one more image than the minumum.
    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

    // We should also make sure to not exceed the maximum number of images while
    // doing this, where 0 is a special value that means that there is no maximum.
    if (surfaceCapabilities.maxImageCount > 0 && 
        imageCount > surfaceCapabilities.maxImageCount) {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    return imageCount;
}

void 
SwapChain::initImagesAndViews() {
    assert(mSwapChainImages.empty());
    assert(mSwapChain.get() != VK_NULL_HANDLE);

    mSwapChainImages = LogicalDevice::device().getSwapchainImagesKHR(mSwapChain.get());

    vk::ImageViewCreateInfo info;
    info.setViewType(vk::ImageViewType::e2D);
    info.setFormat(mImageFormat);
    vk::ImageSubresourceRange subresourceRange;
    subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    subresourceRange.setLayerCount(1);
    subresourceRange.setLevelCount(1);
    info.setSubresourceRange(subresourceRange);

    const uint32_t swapChainImageCount = static_cast<uint32_t>(mSwapChainImages.size());
    mSwapChainImageViews.resize(swapChainImageCount);
    for (uint32_t i = 0; i < swapChainImageCount; ++i) {
        info.setImage(mSwapChainImages[i]);
        mSwapChainImageViews[i] = LogicalDevice::device().createImageViewUnique(info);
    }
}

void
SwapChain::initViewportAndScissorRect() {
    assert(mSwapChain.get() != VK_NULL_HANDLE);

    mViewport.setWidth(static_cast<float>(mExtent.width));
    mViewport.setHeight(static_cast<float>(mExtent.height));
    mViewport.setMinDepth(0.0f);
    mViewport.setMaxDepth(1.0f);

    // We want to draw to the entire framebuffer, so we 
    // will specify a scissor rectangle that covers it entirely.
    mScissorRect.setExtent(mExtent);
}

void
SwapChain::initSwapChain() {
    const vk::SurfaceCapabilitiesKHR surfaceCapabilities =
        PhysicalDevice::device().getSurfaceCapabilitiesKHR(Window::surface());

    mExtent = swapChainExtent(surfaceCapabilities, 
                              Window::width(),
                              Window::height());

    const vk::SurfaceFormatKHR surfaceFormat =
        bestFitSurfaceFormat(PhysicalDevice::device().getSurfaceFormatsKHR(Window::surface()));
    mImageFormat = surfaceFormat.format;
    
    vk::SwapchainCreateInfoKHR info;
    info.setSurface(Window::surface());
    info.setMinImageCount(swapChainImageCount(surfaceCapabilities));
    info.setImageFormat(mImageFormat);
    info.setImageColorSpace(surfaceFormat.colorSpace);
    info.setImageExtent(mExtent);
    info.setImageArrayLayers(1);
    info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
    info.setImageSharingMode(vk::SharingMode::eExclusive);
    info.setPresentMode(bestFitPresentMode(PhysicalDevice::device().getSurfacePresentModesKHR(Window::surface())));
    info.setClipped(VK_TRUE);

    const uint32_t graphicsQueueFamilyIndex = PhysicalDevice::graphicsQueueFamilyIndex();
    const uint32_t presentationQueueFamilyIndex = PhysicalDevice::presentationQueueFamilyIndex();
    const uint32_t transferQueueFamilyIndex = PhysicalDevice::transferQueueFamilyIndex();

    std::vector<uint32_t> queueFamilyIndices;
    if (graphicsQueueFamilyIndex == presentationQueueFamilyIndex) {
        if (presentationQueueFamilyIndex == transferQueueFamilyIndex) {
            // Single queue family
            info.setImageSharingMode(vk::SharingMode::eExclusive);
            info.setQueueFamilyIndexCount(0);
            info.setPQueueFamilyIndices(nullptr);
        } else {
            // Graphics and presentation queue family + transfer queue family
            queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
            queueFamilyIndices.push_back(transferQueueFamilyIndex);
            info.setImageSharingMode(vk::SharingMode::eConcurrent);
            info.setQueueFamilyIndexCount(2);
            info.setPQueueFamilyIndices(queueFamilyIndices.data());
        }
    } else if (presentationQueueFamilyIndex == transferQueueFamilyIndex) {
        // Presentation and transfer queue family + graphics queue family
        queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
        queueFamilyIndices.push_back(transferQueueFamilyIndex);
        info.setImageSharingMode(vk::SharingMode::eConcurrent);
        info.setQueueFamilyIndexCount(2);
        info.setPQueueFamilyIndices(queueFamilyIndices.data());
    } else if (graphicsQueueFamilyIndex == transferQueueFamilyIndex) {
        // Graphics and transfer queue family + presentation queue family
        queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
        queueFamilyIndices.push_back(presentationQueueFamilyIndex);
        info.setImageSharingMode(vk::SharingMode::eConcurrent);
        info.setQueueFamilyIndexCount(2);
        info.setPQueueFamilyIndices(queueFamilyIndices.data());
    } else {
        // One queue family for each.
        queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
        queueFamilyIndices.push_back(presentationQueueFamilyIndex);
        queueFamilyIndices.push_back(transferQueueFamilyIndex);
        info.setImageSharingMode(vk::SharingMode::eConcurrent);
        info.setQueueFamilyIndexCount(3);
        info.setPQueueFamilyIndices(queueFamilyIndices.data());
    }

    mSwapChain = LogicalDevice::device().createSwapchainKHRUnique(info);
}
}