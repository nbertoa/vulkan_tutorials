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
	
	// The instance is the connection between our application and the Vulkan library.
	VkInstanceCreateInfo instanceInfo = {};	
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;

	// Set instance extensions
	const std::vector<const char*> instanceExtensions = getInstanceExtensions();
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.empty() ? nullptr : instanceExtensions.data();
	
	// Get instance layers
	const std::vector<const char*> instanceLayers = getInstanceLayers();
	instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
	instanceInfo.ppEnabledLayerNames = instanceLayers.empty() ? nullptr : instanceLayers.data();
	
	vkChecker(vkCreateInstance(&instanceInfo, nullptr, &mInstance));
}

AppInstance::~AppInstance() {
	assert(mInstance != nullptr);
	vkDestroyInstance(mInstance, nullptr);
}

std::vector<const char*>
AppInstance::getInstanceLayers() {
	std::vector<const char*> instanceLayers;
#ifndef NDEBUG // Debug
	instanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");
	assert(areInstanceLayersSupported(instanceLayers));
#endif

	return instanceLayers;
}

bool 
AppInstance::areInstanceLayersSupported(const std::vector<const char*>& layers) {
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

std::vector<const char*>
AppInstance::getInstanceExtensions() {	
	// Get extensions required by GLFW to be able to Vulkan surfaces for GLFW windows.
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	assert(glfwExtensions != nullptr);

	std::vector<const char*> instanceExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
#ifndef NDEBUG // Debug
	instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return instanceExtensions;
}
}