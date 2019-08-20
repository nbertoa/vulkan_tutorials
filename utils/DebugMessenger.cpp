#include "DebugMessenger.h"

#include <cassert>
#include <iostream>

#include "AppInstance.h"
#include "DebugUtils.h"

namespace vk {
DebugMessenger::DebugMessenger(const AppInstance& appInstance,
                               const VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo)
    : mAppInstance(appInstance)
{
    // The function to create a debug utils messenger is an extension function, 
    // so it is not automatically loaded. We need to look up its address
    // ourselves.
    PFN_vkCreateDebugUtilsMessengerEXT function =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mAppInstance.vkInstance(),
                                                                                   "vkCreateDebugUtilsMessengerEXT"));
    assert(function);
    vkChecker(function(mAppInstance.vkInstance(), 
                       &messengerCreateInfo, 
                       nullptr, 
                       &mMessenger));
}

DebugMessenger::~DebugMessenger() {
    assert(mMessenger != VK_NULL_HANDLE);

    // The function to destroy a debug utils messenger is an extension function, 
    // so it is not automatically loaded. We need to look up its address
    // ourselves.
    PFN_vkDestroyDebugUtilsMessengerEXT function =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mAppInstance.vkInstance(),
                                                                                    "vkDestroyDebugUtilsMessengerEXT"));
    assert(function);
    function(mAppInstance.vkInstance(), 
             mMessenger, 
             nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugMessenger::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
                              VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
                              const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                              void* /*userData*/) {
    assert(callbackData != nullptr);
    std::cerr << "Validation layer: " << callbackData->pMessage << std::endl;
    return VK_FALSE;
}
}