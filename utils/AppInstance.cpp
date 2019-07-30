#include "AppInstance.h"

#include "utils/ErrorChecks.h"

namespace vk {
AppInstance::AppInstance() {
	// This is optional but it may provide some useful information to the driver to optimize 
	// for our specific application, for example, because it ises a well-known graphics 
	// engine with certain special behavior,
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = nullptr;
	appInfo.pEngineName = nullptr;
	appInfo.apiVersion = 0;

	// Get extensions required by GLFW to be able to Vulkan surfaces for GLFW windows.
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	assert(glfwExtensions != nullptr);
	
	VkInstanceCreateInfo instanceInfo = {};
#ifdef NDEBUG
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.ppEnabledLayerNames = nullptr;
#else
	std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	setValidationLayers(validationLayers, instanceInfo);
#endif

	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = glfwExtensionCount;
	instanceInfo.ppEnabledExtensionNames = glfwExtensions;	
	vkChecker(vkCreateInstance(&instanceInfo, nullptr, &mInstance));
}

AppInstance::~AppInstance() {
	assert(mInstance != nullptr);
	vkDestroyInstance(mInstance, nullptr);
}

void
AppInstance::setValidationLayers(const std::vector<const char*>& validationLayers, VkInstanceCreateInfo& instanceInfo) {
	assert(checkValidationLayerSupport(validationLayers));
	instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	instanceInfo.ppEnabledLayerNames = validationLayers.data();
}

bool 
AppInstance::checkValidationLayerSupport(const std::vector<const char*>& layers) {
	uint32_t layerCount;
	vkChecker(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkChecker(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

	for (const char* layerName : layers) {
		assert(layerName != nullptr);
		bool layerFound = false;

		for (const VkLayerProperties& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (layerFound == false) {
			return false;
		}
	}

	return true;
}
}