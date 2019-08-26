#ifndef UTILS_DEBUG_MESSENGER
#define UTILS_DEBUG_MESSENGER

#include <vulkan/vulkan.h>

namespace vk {
class AppInstance;

// VkDebugUtilsMessengerEXT wrapper to be able to create/destroy/set its callback easily.
class DebugMessenger {
public:
    DebugMessenger(const AppInstance& mAppInstance,
                   const VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo);
    ~DebugMessenger();
    DebugMessenger(DebugMessenger&& other) noexcept;

    DebugMessenger(const DebugMessenger&) = delete;
    const DebugMessenger& operator=(const DebugMessenger&) = delete;

    // Instance layers have no way to relay the messages back to our program. 
    // To receive those messages, we need to create a callback function.
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                        void* userData);

private:
    const AppInstance& mAppInstance;
    VkDebugUtilsMessengerEXT mMessenger = VK_NULL_HANDLE;
};
}

#endif