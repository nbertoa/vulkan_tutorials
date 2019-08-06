#include "SwapChain.h"

#include <algorithm>
#include <cassert>
#include <limits>

#include "Window.h"
#include "WindowSurface.h"

namespace vk {
SwapChain::SwapChain(const Window& window, const WindowSurface& windowSurface, const VkPhysicalDevice physicalDevice) {
	assert(physicalDevice != VK_NULL_HANDLE);

	uint32_t windowWidth;
	uint32_t windowHeight;
	window.widthAndHeight(windowWidth, windowHeight);

	const VkSurfaceFormatKHR surfaceFormat = swapChainSurfaceFormat(windowSurface.surfaceFormats(physicalDevice));
	const VkPresentModeKHR presentMode = swapChainPresentMode(windowSurface.presentModes(physicalDevice));
	const VkExtent2D extent = swapChainExtent(windowSurface.surfaceCapabilities(physicalDevice), windowWidth, windowHeight);
}

VkSurfaceFormatKHR 
SwapChain::swapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) {
	assert(surfaceFormats.empty() == false);

	for (const VkSurfaceFormatKHR& surfaceFormat : surfaceFormats) {
		if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
			surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return surfaceFormat;
		}
	}

	return surfaceFormats[0];
}

VkPresentModeKHR 
SwapChain::swapChainPresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
	// Some drivers currently do not properly support VK_PRESENT_MODE_FIFO_KHR.
	// So we should prefer VK_PRESENT_MODE_IMMEDIATE_KHR if VK_PRESENT_MODE_MAILBOX_KHR
	// is not available.

	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const VkPresentModeKHR presentMode : presentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
		else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	return bestMode;
}

VkExtent2D 
SwapChain::swapChainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, const uint32_t windowWidth, const uint32_t windowHeight) {
	// Vulkan tells us to match the resolution of the window by setting the width and height in the currentExtent member.
	// However, some window managers do allow us to differ here and this is indicated by setting the width
	// and height in currentExtent to a special value. It is the maximum value of uint32_t.
	// In that case, we will pick the resolution that best matches the window within the minImageExtent and maxImageExtent bounds.

	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return surfaceCapabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { windowWidth , windowHeight };

		actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}
}