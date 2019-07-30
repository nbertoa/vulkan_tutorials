#ifndef UTILS_APP_INSTANCE
#define UTILS_APP_INSTANCE

#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class AppInstance {
public:
	// Preconditions:
	// - GLFW must be initialized.
	AppInstance();
	~AppInstance();

	const VkInstance& vkInstance() const { assert(mInstance != nullptr);  return mInstance; }

private:
	static void setValidationLayers(const std::vector<const char*>& validationLayers, VkInstanceCreateInfo& instanceInfo);
	static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

	VkInstance mInstance = nullptr;
};
}

#endif