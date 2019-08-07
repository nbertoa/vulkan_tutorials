#ifndef UTILS_SWAP_CHAIN
#define UTILS_SWAP_CHAIN

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class Window;
class WindowSurface;

// VkSwapChain wrapper to be able to create/destroy/get swap chain easily.
// The swapchain provides the ability to present rendering results
// to a surface. A swapchain is an abstraction for an array of
// presentable images that are associated with a surface.
// The swapchain images are represented by VkImage objects.
class SwapChain {
public:
	SwapChain(const Window& window, const WindowSurface& windowSurface, const VkPhysicalDevice physicalDevice);
	
private:
	static VkSurfaceFormatKHR swapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);

	// The swap chain presentation mode is the most importatn setting for the swap chain because 
	// it represents the actual conditions for showing images to the screen.
	static VkPresentModeKHR swapChainPresentMode(const std::vector<VkPresentModeKHR>& presentModes);

	// The swap extent is the resolution of the swap chain images and it is almost always exactly equal to
	// the resolution of the window that we are drawing to.
	static VkExtent2D swapChainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, const uint32_t windowWidth, const uint32_t windowHeight);

	static uint32_t swapChainImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

	VkSwapchainKHR mSwapChain = VK_NULL_HANDLE;
	VkSurfaceFormatKHR mSurfaceFormat;
	VkExtent2D mExtent;
};
}

#endif