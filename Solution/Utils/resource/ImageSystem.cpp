#include "ImageSystem.h"

#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Image.h"

namespace vk2 {
ImageSystem::ImageByPath 
ImageSystem::mImageByPath = {};

Image&
ImageSystem::getOrLoadImage(const std::string& imageFilePath,
                            const CommandPool& transferCommandPool) {
    Image* image = nullptr;

    // Check if the image was already loaded.
    ImageByPath::const_iterator findIt = mImageByPath.find(imageFilePath);
    if (findIt != mImageByPath.end()) {
        image = findIt->second;
        assert(image != nullptr);
    } else {
        int textureWidth = 0;
        int textureHeight = 0;
        int textureChannels = 0;
        stbi_uc* imageData = stbi_load(imageFilePath.c_str(),
                                       &textureWidth,
                                       &textureHeight,
                                       &textureChannels,
                                       STBI_rgb_alpha);

        assert(imageData != nullptr);

        const VkDeviceSize imageSize = static_cast<VkDeviceSize>(textureWidth) * textureHeight * 4;

        image = new Image(textureWidth,
                          textureHeight,
                          VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
                          VK_IMAGE_USAGE_SAMPLED_BIT,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        image->copyFromDataToDeviceMemory(imageData,
                                          imageSize,
                                          transferCommandPool);

        mImageByPath[imageFilePath] = image;
    }

    assert(image != nullptr);

    return *image;
}

void
ImageSystem::eraseImage(const std::string& imageFilePath) {
    ImageByPath::const_iterator findIt = mImageByPath.find(imageFilePath);
    if (findIt != mImageByPath.end()) {
        const Image* image = findIt->second;
        assert(image != nullptr);
        delete image;
        mImageByPath.erase(findIt);
    }
}

void
ImageSystem::clear() {
    for (const auto& pathAndImage : mImageByPath) {
        delete pathAndImage.second;
    }
}
}