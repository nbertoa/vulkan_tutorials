#ifndef UTILS_APP_INSTANCE
#define UTILS_APP_INSTANCE

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class DebugMessenger;

// VkInstance wrappers with methods to create/destroy/get it.
class AppInstance {
public:
	// Preconditions:
	// - GLFW must be initialized.
	AppInstance();
	~AppInstance();

	const VkInstance& vkInstance() const { assert(mInstance != nullptr);  return mInstance; }

private:
	void createInstance(const VkDebugUtilsMessengerCreateInfoEXT* debugMessengerInfo);

	// Validation layers are optional components that hook into Vulkan function calls
	// to apply additional operations. We use them for debugging functionality.
	static std::vector<const char*> getInstanceLayers();
	static bool areInstanceLayersSupported(const std::vector<const char*>& instanceLayers);

	// Vulkan is a platform agnostic API, which means that we need an extension
	// to interface with different systems, like for example, the window system.
	static std::vector<const char*> getRequiredInstanceExtensions();

#ifndef NDEBUG // Debug
	static VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo();
#endif

	VkInstance mInstance = VK_NULL_HANDLE;	

#ifndef NDEBUG // Debug
	DebugMessenger* mMessenger = nullptr;
#endif
};
}

#endif