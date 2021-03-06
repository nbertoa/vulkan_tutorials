#include "DebugMessenger.h"

#include <cassert>
#include <iostream>

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

namespace vulkan {
DebugMessenger::DebugMessenger() {
    // The function to create a debug utils messenger is an extension function, 
    // so it is not automatically loaded. 
    // We need to look up its address ourselves.
    PFN_vkCreateDebugUtilsMessengerEXT function =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(Instance::instance(),
                                  "vkCreateDebugUtilsMessengerEXT"));
    assert(function);
    const vk::DebugUtilsMessengerCreateInfoEXT info = messengerCreateInfo();
    function(Instance::instance(),
            reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&info),
            nullptr, 
            reinterpret_cast<VkDebugUtilsMessengerEXT*>(&mMessenger));
}

DebugMessenger::~DebugMessenger() {
    if (mMessenger == VK_NULL_HANDLE) {
        return;
    }

    // The function to destroy a debug utils messenger is an extension function, 
    // so it is not automatically loaded. 
    // We need to look up its address ourselves.
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
    vk::DebugUtilsMessengerCreateInfoEXT info; 
    info.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
    info.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    info.setPfnUserCallback(debugCallback);

    return info;
}
}