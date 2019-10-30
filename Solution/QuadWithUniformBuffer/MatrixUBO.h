#ifndef MATRIX_UBO
#define MATRIX_UBO

#include <glm/glm.hpp>

struct MatrixUBO {
    glm::mat4 mModelMatrix;
    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;

    void update(const uint32_t swapChainImageIndex,
                const float swapChainImageAspectRatio);
};

#endif 