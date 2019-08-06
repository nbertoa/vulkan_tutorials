#ifndef UTILS_SWAP_CHAIN
#define UTILS_SWAP_CHAIN

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class Window;
class WindowSurface;

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
};
}

#endif