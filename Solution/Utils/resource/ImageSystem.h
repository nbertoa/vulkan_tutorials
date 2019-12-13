#ifndef UTILS_RESOURCE_IMAGE_SYSTEM
#define UTILS_RESOURCE_IMAGE_SYSTEM

#include <string>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace vulkan {
class Image;

class ImageSystem {
public:
    ImageSystem() = delete;
    ~ImageSystem() = delete;
    ImageSystem(ImageSystem&&) noexcept = delete;
    ImageSystem(const ImageSystem&) = delete;
    const ImageSystem& operator=(const ImageSystem&) = delete;

    static Image&
    getOrLoadImage(const std::string& imageFilePath,
                   const vk::CommandPool transferCommandPool);

    static void
    eraseImage(const std::string& imageFilePath);

    static void
    clear();

private:
    using ImageByPath = std::unordered_map<std::string, Image*>;
    static ImageByPath mImageByPath;
};
}

#endif