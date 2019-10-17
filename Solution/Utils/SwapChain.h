#ifndef UTILS_SWAP_CHAIN
#define UTILS_SWAP_CHAIN

#include <limits>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;
class PhysicalDevice;
class Semaphore;
class Surface;

//
// VkSwapChain wrapper.
//
// The swapchain provides the ability to present rendering results
// to a surface. A swapchain is an abstraction for an array of
// presentable images that are associated with a surface.
// The swapchain images are represented by VkImage objects.
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
// From the SwapChain you can query it for the VkImages it contains.
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
    // * physicalDevice and surface are used to get properties
    //   like surface capabilities, formats, and present modes,
    //   needed for swap chain creation.
    SwapChain(const LogicalDevice& logicalDevice,
              const PhysicalDevice& physicalDevice,
              const uint32_t imageWidth,
              const uint32_t imageHeight,
              const Surface& surface);
    ~SwapChain();
    SwapChain(SwapChain&& other) noexcept;
    SwapChain(const SwapChain&) = delete;
    const SwapChain& operator=(const SwapChain&) = delete;

    // Retrieve the index of the next available presentable image.
    //
    // * semaphore will become signaled when the presentation engine
    // has released ownership of the image.
    uint32_t 
    acquireNextImage(const Semaphore& semaphore);

    // Precondition:
    // * acquireNextImage must have been called at least once.
    uint32_t 
    currentImageIndex() const;

    // * waitSemaphore is the semaphore to wait for before
    //   issuing the present request.
    void present(const Semaphore& waitSemaphore,
                 const uint32_t imageIndex);

    // The viewport describes the region of the framebuffer that the output
    // will be rendered too.
    // It defines the transformation from the image to the framebuffer.
    //
    // VkViewport:
    // - x and y are the viewport’s upper left corner(x, y).
    // - width and height are the viewport’s widthand height, respectively.
    // - minDepth and maxDepth are the depth range for the viewport.
    //   It is valid for minDepth to be greater than or equal to maxDepth.
    const VkViewport& 
    viewport() const;

    // While viewports define the transformation from the image to the framebuffer,
    // scissor rectangles define in which regions pixels will actually be stored.
    // Any pixels outside the scissor rectangles will be discarded by the rasterizer.
    //
    // VkRect2D:
    // Rectangles are used to describe a specified rectangular region of pixels within 
    // an image or framebuffer. Rectangles include both an offset and an extent of the 
    // same dimensionality.
    // - offset is a VkOffset2D specifying the rectangle offset.
    // - extent is a VkExtent2D specifying the rectangle extent.
    const VkRect2D& 
    scissorRect() const;

    // VkPipelineViewportStateCreateInfo:
    // - flags is reserved for future use.
    // - viewportCount is the number of viewports used by the pipeline.
    // - pViewports is a pointer to an array of VkViewport structures, defining the viewport transforms.
    //   If the viewport state is dynamic, this member is ignored.
    // - scissorCount is the number of scissorsand must match the number of viewports.
    // - pScissors is a pointer to an array of VkRect2D structures defining the rectangular bounds 
    //   of the scissor for the corresponding viewport. If the scissor state is dynamic, this member is ignored.
    VkPipelineViewportStateCreateInfo 
    pipelineViewportCreateInfo() const;

    VkFormat 
    imageFormat() const;

    size_t 
    imageViewCount() const;

    // Image objects are not directly accessed by pipeline shaders for reading or writing image data. 
    // Instead, image views representing contiguous ranges of the image subresources and containing 
    // additional metadata are used for that purpose. 
    // Views must be created on images of compatible types, and must represent a valid subset of image subresources.
    // VkImageView is a set of parameters referring to a specific image.
    // There you can interpret pixels as having some other(compatible) format, swizzle any components, 
    // and limit the view to a specific range of MIP levels or array layers.
    const std::vector<VkImageView>& 
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
    //
    // VkExtent2D:
    // - width is the width of the extent.
    // - height is the height of the extent.
    const VkExtent2D& 
    imageExtent() const;

private:
    // VkSurfaceFormatKHR:
    // - format of the surface (VkFormat)
    // - colorSpace of the surface (VkColorSpaceKHR)
    static VkSurfaceFormatKHR 
    bestFitSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);

    // The swap chain presentation mode is the most important setting for the swap chain because 
    // it represents the actual conditions for showing images to the screen.
    //
    // *presentModes:
    //
    //   - VK_PRESENT_MODE_IMMEDIATE_KHR specifies that the presentation engine does 
    //     not wait for a vertical blanking period to update the current image, meaning 
    //     this mode may result in visible tearing.No internal queuing of presentation 
    //     requests is needed, as the requests are applied immediately.
    //   - VK_PRESENT_MODE_MAILBOX_KHR specifies that the presentation engine waits for 
    //     the next vertical blanking period to update the current image.Tearing cannot be observed.
    //     An internal single - entry queue is used to hold pending presentation requests.
    //     If the queue is full when a new presentation request is received, the new request 
    //     replaces the existing entry, and any images associated with the prior entry become 
    //     available for re - use by the application.
    //     One request is removed from the queueand processed during each vertical blanking 
    //     period in which the queue is non - empty.
    //   - VK_PRESENT_MODE_FIFO_KHR specifies that the presentation engine waits for the 
    //     next vertical blanking period to update the current image.Tearing cannot be observed.
    //     An internal queue is used to hold pending presentation requests.
    //     New requests are appended to the end of the queue, and one request is removed from the 
    //     beginning of the queueand processed during each vertical blanking period in which the 
    //     queue is non - empty.This is the only value of presentMode that is required to be supported.
    //   - VK_PRESENT_MODE_FIFO_RELAXED_KHR specifies that the presentation engine generally 
    //     waits for the next vertical blanking period to update the current image.
    //     If a vertical blanking period has already passed since the last update of the current 
    //     image then the presentation engine does not wait for another vertical blanking period 
    //     for the update, meaning this mode may result in visible tearing in this case.
    //     This mode is useful for reducing visual stutter with an application that will mostly 
    //     present a new image before the next vertical blanking period, but may occasionally be 
    //     late, and present a new image just after the next vertical blanking period.
    //     An internal queue is used to hold pending presentation requests.
    //     New requests are appended to the end of the queue, and one request is removed from the 
    //     beginning of the queueand processed during or after each vertical blanking period in which 
    //     the queue is non - empty.
    //   - VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR specifies that the presentation engine and 
    //     application have concurrent access to a single image, which is referred to as a shared 
    //     presentable image. The presentation engine is only required to update the current 
    //     image after a new presentation request is received.
    //     Therefore the application must make a presentation request whenever an update is required.
    //     However, the presentation engine may update the current image at any point, meaning 
    //     this mode may result in visible tearing.
    //   - VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR specifies that the presentation engine
    //     and application have concurrent access to a single image, which is referred to as a 
    //     shared presentable image. The presentation engine periodically updates the current image 
    //     on its regular refresh cycle.
    //     The application is only required to make one initial presentation request, 
    //     after which the presentation engine must update the current image without any need for 
    //     further presentation requests.
    //     The application can indicate the image contents have been updated by making a presentation 
    //     request, but this does not guarantee the timing of when it will be updated.
    //     This mode may result in visible tearing if rendering to the image is not timed correctly.
    static VkPresentModeKHR 
    bestFitPresentMode(const std::vector<VkPresentModeKHR>& presentModes);

    // The swap extent is the resolution of the swap chain images and it is almost always exactly equal to
    // the resolution of the window that we are drawing to.
    //
    // * surfaceCapabilities:
    //   - minImageCount is the minimum number of images the specified device supports for 
    //     a swapchain created for the surface, and will be at least one.
    //   - maxImageCount is the maximum number of images the specified device supports for a swapchain 
    //     created for the surface, and will be either 0, or greater than or equal to minImageCount. 
    //     A value of 0 means that there is no limit on the number of images, though there may be limits related 
    //     to the total amount of memory used by presentable images.
    //   - currentExtent is the current width and height of the surface, or the special value 
    //     [eq]#(0xFFFFFFFF, 0xFFFFFFFF)# indicating that the surface size will be determined by 
    //     the extent of a swapchain targeting the surface.
    //   - minImageExtent is the current width and height of the surface, or the special value 
    //     [eq]#(0xFFFFFFFF, 0xFFFFFFFF)# indicating that the surface size will be determined by 
    //     the extent of a swapchain targeting the surface.
    //   - maxImageExtent contains the largest valid swapchain extent for the surface on the specified device. 
    //     The width and height of the extent will each be greater than or equal to the corresponding width and 
    //     height of minImageExtent. The width and height of the extent will each be greater than or equal to the 
    //     corresponding width and height of currentExtent, unless currentExtent has the special value described above.
    //   - maxImageArrayLayers is the maximum number of layers presentable images can have for a swapchain created 
    //     for this device and surface, and will be at least one.
    //   - supportedTransforms is a bitmask of VkSurfaceTransformFlagBitsKHR indicating the presentation transforms 
    //     supported for the surface on the specified device. At least one bit will be set.
    //   - currentTransform is VkSurfaceTransformFlagBitsKHR value indicating the surface's current transform 
    //     relative to the presentation engine's natural orientation.
    //   - supportedCompositeAlpha is a bitmask of VkCompositeAlphaFlagBitsKHR, representing the alpha compositing modes 
    //     supported by the presentation engine for the surface on the specified device, and at least one bit will be set. 
    //     Opaque composition can be achieved in any alpha compositing mode by either using an image format that has 
    //     no alpha component, or by ensuring that all pixels in the presentable images have an alpha value of 1.0.
    //   - is a bitmask of VkImageUsageFlagBits representing the ways the application can use the presentable images of 
    //     a swapchain created with VkPresentModeKHR set to VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_MAILBOX_KHR, 
    //     VK_PRESENT_MODE_FIFO_KHR or VK_PRESENT_MODE_FIFO_RELAXED_KHR
    static VkExtent2D 
    swapChainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, 
                    const uint32_t windowWidth,
                    const uint32_t windowHeight);

    static uint32_t 
    swapChainImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);
    
    void 
    initSwapChain(const uint32_t imageWidth,
                  const uint32_t imageHeight,
                  const Surface& surface,
                  const PhysicalDevice& physicalDevice);

    void 
    initImagesAndViews();
    
    void 
    initViewportAndScissorRect();

    const LogicalDevice& mLogicalDevice;
    
    VkSwapchainKHR mSwapChain = VK_NULL_HANDLE;

    std::vector<VkImage> mSwapChainImages;
    std::vector<VkImageView> mSwapChainImageViews;

    uint32_t mCurrentImageIndex = std::numeric_limits<uint32_t>::max();

    VkFormat mImageFormat = {};
    VkExtent2D mExtent = {};

    VkViewport mViewport = {};
    VkRect2D mScissorRect = {};
};
}

#endif