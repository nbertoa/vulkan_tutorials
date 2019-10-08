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
    std::cerr << "Validation layer: " << callbackData->pMessage << std::endl;
    return VK_FALSE;
}
}

namespace vk {
DebugMessenger::DebugMessenger(const Instance& instance)
    : mInstance(instance)
{
    // The function to create a debug utils messenger is an extension function, 
    // so it is not automatically loaded. We need to look up its address
    // ourselves.
    PFN_vkCreateDebugUtilsMessengerEXT function =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mInstance.vkInstance(),
                                                                                   "vkCreateDebugUtilsMessengerEXT"));
    assert(function);
    const VkDebugUtilsMessengerCreateInfoEXT createInfo = messengerCreateInfo();
    vkChecker(function(mInstance.vkInstance(),
                       &createInfo,
                       nullptr, 
                       &mMessenger));
}

DebugMessenger::~DebugMessenger() {
    if (mMessenger == VK_NULL_HANDLE) {
        return;
    }

    // The function to destroy a debug utils messenger is an extension function, 
    // so it is not automatically loaded. We need to look up its address
    // ourselves.
    PFN_vkDestroyDebugUtilsMessengerEXT function =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mInstance.vkInstance(),
                                                                                    "vkDestroyDebugUtilsMessengerEXT"));
    assert(function);
    function(mInstance.vkInstance(),
             mMessenger, 
             nullptr);
}

DebugMessenger::DebugMessenger(DebugMessenger&& other) noexcept
    : mInstance(other.mInstance)
    , mMessenger(other.mMessenger)
{
    other.mMessenger = VK_NULL_HANDLE;
}

VkDebugUtilsMessengerCreateInfoEXT
DebugMessenger::messengerCreateInfo() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // Specify all the types of severities you would like 
    // your callback to be called for.
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    // Filter which types of messages your callback
    // is notified about.
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    // Pointer to the callback function. 
    createInfo.pfnUserCallback = debugCallback;

    return createInfo;
}
}