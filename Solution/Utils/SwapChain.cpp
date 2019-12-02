#include "SwapChain.h"

#include <algorithm>
#include <cassert>

#include "DebugUtils.h"
#include "Window.h"
#include "device/LogicalDevice.h"
#include "device/PhysicalDevice.h"

namespace vk2 {
SwapChain::SwapChain() {       
    initSwapChain();
    initImagesAndViews();
    initViewportAndScissorRect();
}

SwapChain::~SwapChain() {
    assert(mSwapChain != VK_NULL_HANDLE);

    for (const VkImageView view : mSwapChainImageViews) {
        assert(view != VK_NULL_HANDLE);
        vkDestroyImageView(LogicalDevice::device(),
                           view,
                           nullptr);
    }

    vkDestroySwapchainKHR(LogicalDevice::device(),
                          mSwapChain, 
                          nullptr);
}

SwapChain::SwapChain(SwapChain&& other) noexcept
    : mSwapChain(other.mSwapChain)
    , mSwapChainImages(std::move(mSwapChainImages))
    , mSwapChainImageViews(std::move(mSwapChainImageViews))
    , mImageFormat(other.mImageFormat)
    , mExtent(other.mExtent)
    , mViewport(other.mViewport)
    , mScissorRect(other.mScissorRect)
{
    other.mSwapChain = VK_NULL_HANDLE;
}

uint32_t 
SwapChain::acquireNextImage(const vk::Semaphore& semaphore) {
    assert(mSwapChain != VK_NULL_HANDLE);

    // - Logical device associated with the swap chain
    // - Swapchain from which an image is being acquired
    // - The third parameter specifies a timeout in nanoseconds 
    //   for an image to become available. Using the maximum
    //   value of a 64 bit unsigned integer disables the timeout.
    // - semaphore that will become signaled when the presentation engine
    //   has released ownership of the image.
    // - fence that will become signaled when the presentation engine
    //   has released ownership of the image.
    // - Image index of the next image to use.
    vkChecker(vkAcquireNextImageKHR(LogicalDevice::device(),
                                    mSwapChain,
                                    std::numeric_limits<uint64_t>::max(),
                                    semaphore,
                                    VK_NULL_HANDLE,
                                    &mCurrentImageIndex));

    return mCurrentImageIndex;
}

uint32_t 
SwapChain::currentImageIndex() const {
    assert(mCurrentImageIndex != std::numeric_limits<uint32_t>::max());
    return mCurrentImageIndex;
}

void
SwapChain::present(const vk::Semaphore& waitSemaphore,
                   const uint32_t imageIndex) {
    assert(mSwapChain != VK_NULL_HANDLE);

    // VkPresentInfoKHR:
    // - waitSemaphoreCount to wait for before issuing the present request.The number may be zero.
    // - pWaitSemaphores is an optional array semaphores with 
    //   waitSemaphoreCount entries, and specifies the semaphores to wait for before 
    //   issuing the present request.
    // - swapchainCount being presented to by this command.
    // - pSwapchains array where a given swapchain must not appear in this list more than once.
    // - pImageIndices into the array of each swapchain’s presentable images, 
    //   with swapchainCount entries. Each entry in this array identifies the image to present on 
    //   the corresponding entry in the pSwapchains array.
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = (VkSemaphore*)&waitSemaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &mSwapChain;
    info.pImageIndices = &imageIndex;

    vkChecker(vkQueuePresentKHR(LogicalDevice::presentationQueue(),
                                &info));
}

const VkViewport& 
SwapChain::viewport() const {
    assert(mSwapChain != VK_NULL_HANDLE);
    return mViewport;
}

const VkRect2D& 
SwapChain::scissorRect() const {
    assert(mSwapChain != VK_NULL_HANDLE);
    return mScissorRect;
}

VkFormat
SwapChain::imageFormat() const {
    assert(mSwapChain != VK_NULL_HANDLE);
    return mImageFormat;
}

uint32_t 
SwapChain::imageViewCount() const {
    assert(mSwapChain != VK_NULL_HANDLE);
    return static_cast<uint32_t>(mSwapChainImageViews.size());
}

const std::vector<VkImageView>&
SwapChain::imageViews() const {
    assert(mSwapChain != VK_NULL_HANDLE);
    return mSwapChainImageViews;
}

uint32_t
SwapChain::imageWidth() const {
    assert(mSwapChain != VK_NULL_HANDLE);
    return mExtent.width;
}

uint32_t
SwapChain::imageHeight() const {
    assert(mSwapChain != VK_NULL_HANDLE);
    return mExtent.height;
}

float 
SwapChain::imageAspectRatio() const {
    assert(mSwapChain != VK_NULL_HANDLE);
    return mExtent.width / static_cast<float>(mExtent.height);
}

const VkExtent2D&
SwapChain::imageExtent() const {
    assert(mSwapChain != VK_NULL_HANDLE);
    return mExtent;
}

VkSurfaceFormatKHR
SwapChain::bestFitSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) {
    assert(surfaceFormats.empty() == false);

    for (const VkSurfaceFormatKHR& surfaceFormat : surfaceFormats) {
        // For the color space we will use SRGB if it is available,
        // because it results in more accurate perceived colors.
        // For the format, we will use the standard RGB.
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return surfaceFormat;
        }
    }

    return surfaceFormats[0];
}

VkPresentModeKHR
SwapChain::bestFitPresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
    // Some drivers currently do not properly support VK_PRESENT_MODE_FIFO_KHR.
    // So we should prefer VK_PRESENT_MODE_IMMEDIATE_KHR if VK_PRESENT_MODE_MAILBOX_KHR
    // is not available.
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for (const VkPresentModeKHR presentMode : presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        } else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            bestMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    return bestMode;
}

VkExtent2D
SwapChain::swapChainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, 
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
        VkExtent2D actualExtent = {windowWidth, windowHeight};

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
SwapChain::swapChainImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) {
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
    assert(mSwapChain != VK_NULL_HANDLE);

    // Images
    uint32_t swapChainImageCount;
    vkChecker(vkGetSwapchainImagesKHR(LogicalDevice::device(),
                                      mSwapChain,
                                      &swapChainImageCount,
                                      nullptr));
    mSwapChainImages.resize(swapChainImageCount);
    vkChecker(vkGetSwapchainImagesKHR(LogicalDevice::device(),
                                      mSwapChain,
                                      &swapChainImageCount,
                                      mSwapChainImages.data()));

    // Image views
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    // How the image data should be interpreted.
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = mImageFormat;
    // Allows you to swizzle the color channels around.
    // For example, you can map all of the channels to the red channel
    // for a monocrhome texture. You can also map constant values
    // of 0 and 1 to a channel.
    // We stick to the default mapping.
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    // Subresource range field describes what the image's purpose
    // is and which part of the image should be accessed.
    // Our images will be used as color targets without any 
    // mipmapping levels or multiple layers.
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    mSwapChainImageViews.resize(swapChainImageCount);
    for (uint32_t i = 0; i < swapChainImageCount; ++i) {
        createInfo.image = mSwapChainImages[i];
        vkChecker(vkCreateImageView(LogicalDevice::device(),
                                    &createInfo,
                                    nullptr,
                                    &mSwapChainImageViews[i]));
    }
}

void
SwapChain::initViewportAndScissorRect() {
    assert(mSwapChain != VK_NULL_HANDLE);

    // This will almost always be (0, 0) and width and height.
    mViewport.x = 0.0f;
    mViewport.y = 0.0f;
    mViewport.width = static_cast<float>(mExtent.width);
    mViewport.height = static_cast<float>(mExtent.height);
    // Specify the range of depth values to use for the framebuffer.
    mViewport.minDepth = 0.0f;
    mViewport.maxDepth = 1.0f;

    // We want to drwa to the entire framebuffer, so we 
    // will specify a scissor rectangle that covers it entirely.
    mScissorRect.offset = {0, 0};
    mScissorRect.extent = mExtent;
}

void
SwapChain::initSwapChain() {
    const VkSurfaceCapabilitiesKHR surfaceCapabilities = 
        Window::physicalDeviceSurfaceCapabilities(PhysicalDevice::vkPhysicalDevice());
    mExtent = swapChainExtent(surfaceCapabilities, 
                              Window::width(),
                              Window::height());

    const VkSurfaceFormatKHR surfaceFormat =
        bestFitSurfaceFormat(Window::physicalDeviceSurfaceFormats(PhysicalDevice::vkPhysicalDevice()));
    mImageFormat = surfaceFormat.format;

    // VkSwapchainCreateInfoKHR:
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
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = Window::vkSurface();
    createInfo.minImageCount = swapChainImageCount(surfaceCapabilities);
    createInfo.imageFormat = mImageFormat;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = mExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode =
        bestFitPresentMode(
            Window::physicalDeviceSurfacePresentModes(PhysicalDevice::vkPhysicalDevice())
        );
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    const uint32_t graphicsQueueFamilyIndex = PhysicalDevice::graphicsQueueFamilyIndex();
    const uint32_t presentationQueueFamilyIndex = PhysicalDevice::presentationQueueFamilyIndex();
    const uint32_t transferQueueFamilyIndex = PhysicalDevice::transferQueueFamilyIndex();

    std::vector<uint32_t> queueFamilyIndices;
    if (graphicsQueueFamilyIndex == presentationQueueFamilyIndex) {
        if (presentationQueueFamilyIndex == transferQueueFamilyIndex) {
            // Single queue family
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        } else {
            // Graphics and presentation queue family + transfer queue family
            queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
            queueFamilyIndices.push_back(transferQueueFamilyIndex);
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
        }
    } else if (presentationQueueFamilyIndex == transferQueueFamilyIndex) {
        // Presentation and transfer queue family + graphics queue family
        queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
        queueFamilyIndices.push_back(transferQueueFamilyIndex);
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    } else if (graphicsQueueFamilyIndex == transferQueueFamilyIndex) {
        // Graphics and transfer queue family + presentation queue family
        queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
        queueFamilyIndices.push_back(presentationQueueFamilyIndex);
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    } else {
        // One queue family for each.
        queueFamilyIndices.push_back(graphicsQueueFamilyIndex);
        queueFamilyIndices.push_back(presentationQueueFamilyIndex);
        queueFamilyIndices.push_back(transferQueueFamilyIndex);
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 3;
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }

    vkChecker(vkCreateSwapchainKHR(LogicalDevice::device(),
                                   &createInfo,
                                   nullptr,
                                   &mSwapChain));
    assert(mSwapChain != VK_NULL_HANDLE);
}
}