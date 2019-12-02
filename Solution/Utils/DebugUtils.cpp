#include "DebugUtils.h"

#include <string>

namespace vk2 {
#ifdef _DEBUG
void vkChecker(const VkResult result) {
    std::string errorMsg;
    switch(result) {
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        errorMsg = "VK_ERROR_OUT_OF_HOST_MEMORY A host memory allocation has failed.";
        break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        errorMsg = "VK_ERROR_OUT_OF_DEVICE_MEMORY A device memory allocation has failed";
        break;
    case VK_ERROR_INITIALIZATION_FAILED:
        errorMsg = "VK_ERROR_INITIALIZATION_FAILED Initialization of an object could not be completed for \
                    implementation - specific reasons";
        break;
    case VK_ERROR_DEVICE_LOST:
        errorMsg = "VK_ERROR_DEVICE_LOST The logical or physical device has been lost";
        break;
    case VK_ERROR_MEMORY_MAP_FAILED:
        errorMsg = "VK_ERROR_MEMORY_MAP_FAILED Mapping of a memory object has failed";
        break;
    case VK_ERROR_LAYER_NOT_PRESENT:
        errorMsg = "VK_ERROR_LAYER_NOT_PRESENT A requested layer is not present or could not be loaded";
        break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        errorMsg = "VK_ERROR_EXTENSION_NOT_PRESENT A requested extension is not supported";
        break;
    case VK_ERROR_FEATURE_NOT_PRESENT:
        errorMsg = "VK_ERROR_FEATURE_NOT_PRESENT A requested feature is not supported";
        break;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        errorMsg = "VK_ERROR_INCOMPATIBLE_DRIVER The requested version of Vulkan is not supported by the driver or \
                    is otherwise incompatible for implementation - specific reasons";
        break;
    case VK_ERROR_TOO_MANY_OBJECTS:
        errorMsg = "VK_ERROR_TOO_MANY_OBJECTS Too many objects of the type have already been created";
        break;
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        errorMsg = "VK_ERROR_FORMAT_NOT_SUPPORTED A requested format is not supported on this device";
        break;
    case VK_ERROR_FRAGMENTED_POOL:
        errorMsg = "VK_ERROR_FRAGMENTED_POOL A pool allocation has failed due to fragmentation of the pool’s \
                    memory. This must only be returned if no attempt to allocate host or device memory was made \
                    to accommodate the new allocation. This should be returned in preference to \
                    VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation \
                    failure was due to fragmentation";
        break;
    case VK_ERROR_SURFACE_LOST_KHR:
        errorMsg = "VK_ERROR_SURFACE_LOST_KHR A surface is no longer available";
        break;
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        errorMsg = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR The requested window is already in use by Vulkan or \
                    another API in a manner which prevents it from being used again";
        break;
    case VK_ERROR_OUT_OF_DATE_KHR:
        errorMsg = "VK_ERROR_OUT_OF_DATE_KHR A surface has changed in such a way that it is no longer compatible \
                    with the swapchain, and further presentation requests using the swapchain will fail. \
                    Applications must query the new surface propertiesand recreate their swapchain if they wish \
                    to continue presenting to the surface";
        break;
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        errorMsg = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR The display used by a swapchain does not use the same \
                    presentable image layout, or is incompatible in a way that prevents sharing an image";
        break;
    case VK_ERROR_INVALID_SHADER_NV:
        errorMsg = "VK_ERROR_INVALID_SHADER_NV One or more shaders failed to compile or link. More details are \
                    reported back to the application via VK_EXT_debug_report if enabled";
        break;
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        errorMsg = "VK_ERROR_OUT_OF_POOL_MEMORY A pool memory allocation has failed. This must only be returned if \
                    no attempt to allocate host or device memory was made to accommodate the new allocation.If \
                    the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be \
                    returned instead";
        break;
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        errorMsg = "VK_ERROR_INVALID_EXTERNAL_HANDLE An external handle is not a valid handle of the specified type";
        break;
    case VK_ERROR_FRAGMENTATION_EXT:
        errorMsg = "VK_ERROR_FRAGMENTATION_EXT A descriptor pool creation has failed due to fragmentation";
        break;
    case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
        errorMsg = "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT A buffer creation failed because the requested address is \
                    not available";
        break;
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        errorMsg = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT An operation on a swapchain created with \
                    VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive fullscreen \
                    access.This may occur due to implementation - dependent reasons, outside of the \
                    application’s control";
        break;
    default:
    // If we reached this point, that is because the VkResult is a success code.
        return;
    }

    assert(false && errorMsg.c_str());
}

void vkChecker(const vk::Result result) {
    std::string errorMsg;
    switch (result) {
    case vk::Result::eErrorOutOfHostMemory:
        errorMsg = "VK_ERROR_OUT_OF_HOST_MEMORY A host memory allocation has failed.";
        break;
    case vk::Result::eErrorOutOfDeviceMemory:
        errorMsg = "VK_ERROR_OUT_OF_DEVICE_MEMORY A device memory allocation has failed";
        break;
    case vk::Result::eErrorInitializationFailed:
        errorMsg = "VK_ERROR_INITIALIZATION_FAILED Initialization of an object could not be completed for \
                    implementation - specific reasons";
        break;
    case vk::Result::eErrorDeviceLost:
        errorMsg = "VK_ERROR_DEVICE_LOST The logical or physical device has been lost";
        break;
    case vk::Result::eErrorMemoryMapFailed:
        errorMsg = "VK_ERROR_MEMORY_MAP_FAILED Mapping of a memory object has failed";
        break;
    case vk::Result::eErrorLayerNotPresent:
        errorMsg = "VK_ERROR_LAYER_NOT_PRESENT A requested layer is not present or could not be loaded";
        break;
    case vk::Result::eErrorExtensionNotPresent:
        errorMsg = "VK_ERROR_EXTENSION_NOT_PRESENT A requested extension is not supported";
        break;
    case vk::Result::eErrorFeatureNotPresent:
        errorMsg = "VK_ERROR_FEATURE_NOT_PRESENT A requested feature is not supported";
        break;
    case vk::Result::eErrorIncompatibleDriver:
        errorMsg = "VK_ERROR_INCOMPATIBLE_DRIVER The requested version of Vulkan is not supported by the driver or \
                    is otherwise incompatible for implementation - specific reasons";
        break;
    case vk::Result::eErrorTooManyObjects:
        errorMsg = "VK_ERROR_TOO_MANY_OBJECTS Too many objects of the type have already been created";
        break;
    case vk::Result::eErrorFormatNotSupported:
        errorMsg = "VK_ERROR_FORMAT_NOT_SUPPORTED A requested format is not supported on this device";
        break;
    case vk::Result::eErrorFragmentedPool:
        errorMsg = "VK_ERROR_FRAGMENTED_POOL A pool allocation has failed due to fragmentation of the pool’s \
                    memory. This must only be returned if no attempt to allocate host or device memory was made \
                    to accommodate the new allocation. This should be returned in preference to \
                    VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation \
                    failure was due to fragmentation";
        break;
    case vk::Result::eErrorSurfaceLostKHR:
        errorMsg = "VK_ERROR_SURFACE_LOST_KHR A surface is no longer available";
        break;
    case vk::Result::eErrorNativeWindowInUseKHR:
        errorMsg = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR The requested window is already in use by Vulkan or \
                    another API in a manner which prevents it from being used again";
        break;
    case vk::Result::eErrorOutOfDateKHR:
        errorMsg = "VK_ERROR_OUT_OF_DATE_KHR A surface has changed in such a way that it is no longer compatible \
                    with the swapchain, and further presentation requests using the swapchain will fail. \
                    Applications must query the new surface propertiesand recreate their swapchain if they wish \
                    to continue presenting to the surface";
        break;
    case vk::Result::eErrorIncompatibleDisplayKHR:
        errorMsg = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR The display used by a swapchain does not use the same \
                    presentable image layout, or is incompatible in a way that prevents sharing an image";
        break;
    case vk::Result::eErrorInvalidShaderNV:
        errorMsg = "VK_ERROR_INVALID_SHADER_NV One or more shaders failed to compile or link. More details are \
                    reported back to the application via VK_EXT_debug_report if enabled";
        break;
    case vk::Result::eErrorOutOfPoolMemory:
        errorMsg = "VK_ERROR_OUT_OF_POOL_MEMORY A pool memory allocation has failed. This must only be returned if \
                    no attempt to allocate host or device memory was made to accommodate the new allocation.If \
                    the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be \
                    returned instead";
        break;
    case vk::Result::eErrorInvalidExternalHandle:
        errorMsg = "VK_ERROR_INVALID_EXTERNAL_HANDLE An external handle is not a valid handle of the specified type";
        break;
    case vk::Result::eErrorFragmentationEXT:
        errorMsg = "VK_ERROR_FRAGMENTATION_EXT A descriptor pool creation has failed due to fragmentation";
        break;
    case vk::Result::eErrorInvalidDeviceAddressEXT:
        errorMsg = "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT A buffer creation failed because the requested address is \
                    not available";
        break;
    case vk::Result::eErrorFullScreenExclusiveModeLostEXT:
        errorMsg = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT An operation on a swapchain created with \
                    VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive fullscreen \
                    access.This may occur due to implementation - dependent reasons, outside of the \
                    application’s control";
        break;
    default:
        // If we reached this point, that is because the VkResult is a success code.
        return;
    }

    assert(false && errorMsg.c_str());
}

#else
void vkChecker(const VkResult) {}
void vkChecker(const vk::Result) {}
#endif
}