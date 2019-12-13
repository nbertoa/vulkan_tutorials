#ifndef UTILS_SWAP_CHAIN
#define UTILS_SWAP_CHAIN

#include <limits>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vulkan {
//
// SwapChain wrapper.
//
// The swapchain provides the ability to present rendering results
// to a surface. A swapchain is an abstraction for an array of
// presentable images that are associated with a surface.
// The swapchain images are represented by vk::Image objects.
//
// One exception to the obligation to allocate and bind DeviceMemory 
// for every Image is the creation of a SwapChain. 
// This is a concept used to present the final image on the screen or 
// inside the window you are drawing into on your operating system. 
// As such, the way of creating it is platform dependent. 
// If you already have a window initialized using a system API, 
// you first need to create a SurfaceKHR object. 
// It needs the Instance object, as well as some system-dependent parameters. 
// For example, on Windows these are: instance handle (HINSTANCE) and window handle (HWND). 
// You can imagine SurfaceKHR object as the Vulkan representation of a window.
//
// From it you can create SwapchainKHR.
// This object requires a LogicalDevice. It represents a set of images that can be presented on 
// the Surface, e.g. using double-or triple-buffering.
// From the SwapChain you can query it for the vk::Images it contains.
// These images already have their backing memory allocated by the system.
//
// You need the SwapChain to:
// - Get the Images and its views
//
// To create/use the SwapChain you need:
// - Surface
//
class SwapChain {
public:
    // Note: The global physical and logical device are used.
    //
    // * The global physical device and surface are used to get properties
    //   like surface capabilities, formats, and present modes,
    //   needed for swap chain creation.
    SwapChain();
    SwapChain(const SwapChain&) = delete;
    const SwapChain& operator=(const SwapChain&) = delete;

    // Retrieve the index of the next available presentable image.
    //
    // * semaphore will become signaled when the presentation engine
    // has released ownership of the image.
    uint32_t 
    acquireNextImage(const vk::Semaphore semaphore);

    // Precondition:
    // * acquireNextImage must have been called at least once.
    uint32_t 
    currentImageIndex() const;

    // * waitSemaphore to wait for before issuing the present request.
    void 
    present(const vk::Semaphore waitSemaphore,
            const uint32_t imageIndex);

    // The viewport describes the region of the framebuffer that the output
    // will be rendered too.
    // It defines the transformation from the image to the framebuffer.
    const vk::Viewport& 
    viewport() const;

    // While viewports define the transformation from the image to the framebuffer,
    // scissor rectangles define in which regions pixels will actually be stored.
    // Any pixels outside the scissor rectangles will be discarded by the rasterizer.
    const vk::Rect2D& 
    scissorRect() const;

    vk::Format 
    imageFormat() const;

    uint32_t
    imageViewCount() const;

    // Image objects are not directly accessed by pipeline shaders for reading or 
    // writing image data. 
    // Instead, image views representing contiguous ranges of the image subresources 
    // and containing additional metadata are used for that purpose. 
    // Views must be created on images of compatible types, and must represent 
    // a valid subset of image subresources.
    // vk::ImageView is a set of parameters referring to a specific image.
    // There you can interpret pixels as having some other(compatible) format, 
    // swizzle any components, 
    // and limit the view to a specific range of MIP levels or array layers.
    const std::vector<vk::UniqueImageView>& 
    imageViews() const;

    uint32_t 
    imageWidth() const;

    uint32_t 
    imageHeight() const;

    float 
    imageAspectRatio() const;

    // The swap extent is the resolution of the swap chain images
    // and it is almost always exactly equal to the resolution of 
    // the window we are drawing to.
    const vk::Extent2D& 
    imageExtent() const;

private:
    static vk::SurfaceFormatKHR 
    bestFitSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats);

    // The swap chain presentation mode is the most important setting for the swap chain because 
    // it represents the actual conditions for showing images to the screen.
    static vk::PresentModeKHR 
    bestFitPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);

    // The swap extent is the resolution of the swap chain images and it is almost always 
    // exactly equal to the resolution of the window that we are drawing to.
    static vk::Extent2D 
    swapChainExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities, 
                    const uint32_t windowWidth,
                    const uint32_t windowHeight);

    static uint32_t 
    swapChainImageCount(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities);
    
    void 
    initSwapChain();

    void 
    initImagesAndViews();
    
    void 
    initViewportAndScissorRect();
        
    vk::UniqueSwapchainKHR mSwapChain;

    std::vector<vk::Image> mSwapChainImages;
    std::vector<vk::UniqueImageView> mSwapChainImageViews;

    uint32_t mCurrentImageIndex = std::numeric_limits<uint32_t>::max();

    vk::Format mImageFormat = {};
    vk::Extent2D mExtent = {};

    vk::Viewport mViewport = {};
    vk::Rect2D mScissorRect = {};
};
}

#endif