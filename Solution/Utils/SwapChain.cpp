#include "SwapChain.h"

#include <algorithm>
#include <cassert>

#include "DebugUtils.h"
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

    vkChecker(LogicalDevice::device().acquireNextImageKHR(mSwapChain.get(),
                                                          std::numeric_limits<uint64_t>::max(),
                                                          semaphore,
                                                          vk::Fence {},
                                                          &mCurrentImageIndex));

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

    vk::PresentInfoKHR info = 
    {
        1,
        &waitSemaphore,
        1,
        &mSwapChain.get(),
        &imageIndex
    };

    vkChecker(LogicalDevice::presentationQueue().presentKHR(info));
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

    for (const vk::SurfaceFormatKHR surfaceFormat : surfaceFormats) {
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

    // Image views
    vk::ImageViewCreateInfo createInfo = 
    {
        vk::ImageViewCreateFlags(),
        vk::Image(), // This will be set later.
        vk::ImageViewType::e2D,
        mImageFormat,
        vk::ComponentMapping(),
        vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor,
                                  0, // base mip level,
                                  1, // level count
                                  0, // base array layer
                                  1} // layer count
    };

    const uint32_t swapChainImageCount = static_cast<uint32_t>(mSwapChainImages.size());
    mSwapChainImageViews.resize(swapChainImageCount);
    for (uint32_t i = 0; i < swapChainImageCount; ++i) {
        createInfo.setImage(mSwapChainImages[i]);
        mSwapChainImageViews[i] = LogicalDevice::device().createImageViewUnique(createInfo);
    }
}

void
SwapChain::initViewportAndScissorRect() {
    assert(mSwapChain.get() != VK_NULL_HANDLE);

    // This will almost always be (0, 0) and width and height.
    mViewport = 
    {
        0.0f, // x 
        0.0f, // y
        static_cast<float>(mExtent.width),
        static_cast<float>(mExtent.height),
        0.0f, // min depth
        1.0f // max depth
    };

    // We want to drwa to the entire framebuffer, so we 
    // will specify a scissor rectangle that covers it entirely.
    mScissorRect = 
    {
        {0, 0}, // offset
        mExtent
    };
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

    // vk::SwapchainCreateInfoKHR:
    // - flags bitmask indicating parameters of the swapchain creation (VK_SWAPCHAIN_CREATE_):
    //    . SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR, PROTECTED_BIT_KHR, MUTABLE_FORMAT_BIT_KHR
    //
    // - surface onto which the swapchain will present images.
    //   If the creation succeeds, the swapchain becomes associated with surface.
    //
    // - minImageCount of presentable images that the application needs.
    //   The implementation will either create the swapchain with at least that many images, 
    //   or it will fail to create the swapchain.
    //
    // - imageFormat the swapchain image(s) will be created with.
    //
    // - imageColorSpace specifying the way the swapchain interprets image data.
    //
    // - imageExtent is the size(in pixels) of the swapchain image(s).
    //   The behavior is platform-dependent if the image extent does not match the surface’s currentExtent as 
    //   returned by vkGetPhysicalDeviceSurfaceCapabilitiesKHR.
    //
    // - imageArrayLayers in a multiview/stereo surface.
    //   For non-stereoscopic-3D applications, this value is 1.
    //
    // - imageUsage bitmask describing the intended usage of 
    //   the(acquired) swapchain images (VK_IMAGE_USAGE_):
    //    . TRANSFER_SRC_BIT, TRANSFER_DST_BIT, SAMPLED_BIT, STORAGE_BIT, COLOR_ATTACHMENT_BIT, 
    //      DEPTH_STENCIL_ATTACHMENT_BIT, TRANSIENT_ATTACHMENT_BIT, INPUT_ATTACHMENT_BIT, 
    //      SHADING_RATE_IMAGE_BIT_NV, FRAGMENT_DENSITY_MAP_BIT_EXT
    //
    // - imageSharingMode for the image(s) of the swapchain.
    //
    // - queueFamilyIndexCount having access to the image(s) of the swapchain when 
    //   imageSharingMode is VK_SHARING_MODE_CONCURRENT.
    //
    // - pQueueFamilyIndices having access to the images(s) 
    //   of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT.
    //
    // - preTransform relative to the presentation engine’s natural orientation, 
    //   applied to the image content prior to presentation.
    //   If it does not match the currentTransform value returned by 
    //   vkGetPhysicalDeviceSurfaceCapabilitiesKHR, the presentation engine will transform 
    //   the image content as part of the presentation operation (VK_SURFACE_TRANSFORM_):
    //   . IDENTITY_BIT_KHR, ROTATE_90_BIT_KHR, ROTATE_180_BIT_KHR, ROTATE_270_BIT_KHR, 
    //     HORIZONTAL_MIRROR_BIT_KHR, HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR, 
    //     HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR, HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR, 
    //     INHERIT_BIT_KHR specifies
    //
    // - compositeAlpha indicating the alpha compositing mode to use when this 
    //   surface is composited together with other surfaces 
    //   on certain window systems (VK_COMPOSITE_ALPHA_):
    //   . OPAQUE_BIT_KHR, PRE_MULTIPLIED_BIT_KHR, POST_MULTIPLIED_BIT_KHR, INHERIT_BIT_KHR
    //
    // - presentMode the swapchain will use.
    //   A swapchain’s present mode determines how incoming present requests will be processed 
    //  and queued internally.
    //
    // - clipped specifies whether the Vulkan implementation is allowed to discard rendering 
    //   operations that  affect regions of the surface that are not visible.
    //   . If set to VK_TRUE, the presentable images associated with the swapchain may not own 
    //     all of their pixels.
    //     Pixels in the presentable images that correspond to regions of the target 
    //     surface obscured by another window 
    //     on the desktop, or subject to some other clipping mechanism will have undefined 
    //     content when read back.
    //     Pixel shaders may not execute for these pixels, and thus any side effects 
    //     they would have had will not occur.
    //     VK_TRUE value does not guarantee any clipping will occur, but allows 
    //     more optimal presentation methods to be used on some platforms.
    //   . If set to VK_FALSE, presentable images associated with the swapchain will own 
    //     all of the pixels they contain.
    //
    // - oldSwapchain is VK_NULL_HANDLE, or the existing non-retired swapchain 
    //   currently associated with surface.
    //   Providing a valid oldSwapchain may aid in the resource reuse, 
    //   and also allows the application to still present any images that are already 
    //   acquired from it.
    vk::SwapchainCreateInfoKHR createInfo = 
    {
        vk::SwapchainCreateFlagsKHR(),
        Window::surface(),
        swapChainImageCount(surfaceCapabilities),
        mImageFormat,
        surfaceFormat.colorSpace,
        mExtent,
        1, // image array layers
        vk::ImageUsageFlagBits::eColorAttachment,
        vk::SharingMode::eExclusive,
        0, // queue family index count
        nullptr, // queue family indices
        vk::SurfaceTransformFlagBitsKHR::eIdentity,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        bestFitPresentMode(PhysicalDevice::device().getSurfacePresentModesKHR(Window::surface())),
        VK_TRUE, // clipped
        nullptr, // old swap chain
    };

    const uint32_t graphicsQueueFamilyIndex = PhysicalDevice::graphicsQueueFamilyIndex();
    const uint32_t presentationQueueFamilyIndex = PhysicalDevice::presentationQueueFamilyIndex();
    const uint32_t transferQueueFamilyIndex = PhysicalDevice::transferQueueFamilyIndex();

    std::vector<uint32_t> queueFamilyIndices;
    if (graphicsQueueFamilyIndex == presentationQueueFamilyIndex) {
        if (presentationQueueFamilyIndex == transferQueueFamilyIndex) {
            // Single queue family
            createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
            createInfo.setQueueFamilyIndexCount(0);
            createInfo.setPQueueFamilyIndices(nullptr);
        } else {
            // Graphics and presentation queue family + transfer queue family
            queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
            queueFamilyIndices.push_back(transferQueueFamilyIndex);
            createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
            createInfo.setQueueFamilyIndexCount(2);
            createInfo.setPQueueFamilyIndices(queueFamilyIndices.data());
        }
    } else if (presentationQueueFamilyIndex == transferQueueFamilyIndex) {
        // Presentation and transfer queue family + graphics queue family
        queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
        queueFamilyIndices.push_back(transferQueueFamilyIndex);
        createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
        createInfo.setQueueFamilyIndexCount(2);
        createInfo.setPQueueFamilyIndices(queueFamilyIndices.data());
    } else if (graphicsQueueFamilyIndex == transferQueueFamilyIndex) {
        // Graphics and transfer queue family + presentation queue family
        queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
        queueFamilyIndices.push_back(presentationQueueFamilyIndex);
        createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
        createInfo.setQueueFamilyIndexCount(2);
        createInfo.setPQueueFamilyIndices(queueFamilyIndices.data());
    } else {
        // One queue family for each.
        queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
        queueFamilyIndices.push_back(presentationQueueFamilyIndex);
        queueFamilyIndices.push_back(transferQueueFamilyIndex);
        createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
        createInfo.setQueueFamilyIndexCount(3);
        createInfo.setPQueueFamilyIndices(queueFamilyIndices.data());
    }

    mSwapChain = LogicalDevice::device().createSwapchainKHRUnique(createInfo);
}
}