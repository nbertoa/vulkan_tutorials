#include "DebugMessenger.h"

#include <cassert>
#include <iostream>

#include "DebugUtils.h"

namespace vk {
DebugMessenger::DebugMessenger(const VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT& messengerCreateInfo)
	: mInstance(instance)
{
	assert(mInstance != VK_NULL_HANDLE);

	// The function to create a debug utils messenger is an extension function, 
	// so it is not automatically loaded. We need to look up its address
	// ourselves.
	PFN_vkCreateDebugUtilsMessengerEXT function =
		reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT"));
	assert(function);
	vkChecker(function(mInstance, &messengerCreateInfo, nullptr, &mMessenger));
}

DebugMessenger::~DebugMessenger() {
	assert(mInstance != VK_NULL_HANDLE);
	assert(mMessenger != VK_NULL_HANDLE);

	// The function to destroy a debug utils messenger is an extension function, 
	// so it is not automatically loaded. We need to look up its address
	// ourselves.
	PFN_vkDestroyDebugUtilsMessengerEXT function = 
		reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT"));
	assert(function);
	function(mInstance, mMessenger, nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL 
DebugMessenger::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
							  VkDebugUtilsMessageTypeFlagsEXT messageType,
							  const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
							  void* userData) {
	assert(callbackData != nullptr);
	std::cerr << "Validation layer: " << callbackData->pMessage << std::endl;
	return VK_FALSE;
}
}