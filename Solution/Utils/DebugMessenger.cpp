#include "DebugMessenger.h"

#include <cassert>
#include <iostream>

#include "DebugUtils.h"
#include "Instance.h"

namespace {
// Instance layers have no way to relay the messages back to our program. 
// To receive those messages, we need to create a callback function.
VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
              VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
              const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
              void* /*userData*/) {
    assert(callbackData != nullptr);
    std::cerr << callbackData->pMessage << std::endl;
    return VK_FALSE;
}
}

namespace vk2 {
DebugMessenger::DebugMessenger() {
    // The function to create a debug utils messenger is an extension function, 
    // so it is not automatically loaded. We need to look up its address
    // ourselves.
    PFN_vkCreateDebugUtilsMessengerEXT function =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(Instance::instance(),
                                  "vkCreateDebugUtilsMessengerEXT"));
    assert(function);
    const vk::DebugUtilsMessengerCreateInfoEXT createInfo = messengerCreateInfo();
    vkChecker(function(Instance::instance(),
                       (VkDebugUtilsMessengerCreateInfoEXT*)&createInfo,
                       nullptr, 
                       (VkDebugUtilsMessengerEXT*)&mMessenger));
}

DebugMessenger::~DebugMessenger() {
    if (mMessenger == VK_NULL_HANDLE) {
        return;
    }

    // The function to destroy a debug utils messenger is an extension function, 
    // so it is not automatically loaded. We need to look up its address
    // ourselves.
    PFN_vkDestroyDebugUtilsMessengerEXT function =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(Instance::instance(),
                                  "vkDestroyDebugUtilsMessengerEXT"));
    assert(function);
    function(Instance::instance(),
             mMessenger, 
             nullptr);
}

vk::DebugUtilsMessengerCreateInfoEXT
DebugMessenger::messengerCreateInfo() {
    vk::DebugUtilsMessengerCreateInfoEXT createInfo = 
    {
        vk::DebugUtilsMessengerCreateFlagsEXT(),
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        debugCallback
    };

    return createInfo;
}
}