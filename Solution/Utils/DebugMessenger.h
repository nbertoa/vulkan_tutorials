#ifndef UTILS_DEBUG_MESSENGER
#define UTILS_DEBUG_MESSENGER

#include <vulkan/vulkan.hpp>

namespace vk2 {
//
// DebugUtilsMessengerEXT wrapper.
//
// A vk::DebugUtilsMessengerEXT is a messenger object which handles passing 
// along debug messages to a provided debug callback.
//
class DebugMessenger {
public:
    DebugMessenger();
    ~DebugMessenger();
    DebugMessenger(const DebugMessenger&) = delete;
    const DebugMessenger& operator=(const DebugMessenger&) = delete;

    static vk::DebugUtilsMessengerCreateInfoEXT 
    messengerCreateInfo();

private:
    vk::DebugUtilsMessengerEXT mMessenger;
};
}

#endif