#ifndef MATRIX_UBO
#define MATRIX_UBO

#include <glm/glm.hpp>

// Vulkan expects the data in your structure to be aligned in memory 
// in a specific way:
// - Scalars have to be aligned by N (N = 4 bytes given 32 bit floats)
// - A vec2 must be aligned by 2N (8 bytes)
// - A vec3 or vec4 must be aligned by 4N (16 bytes)
// - A nested structure must be aligned by the base aligment
//   of its members rounded up to a multiple of 16.
// - A mat4 matrix must have the same alignment as a vec4.

struct MatrixUBO {
    alignas(16) glm::mat4 mModelMatrix;
    alignas(16) glm::mat4 mViewMatrix;
    alignas(16) glm::mat4 mProjectionMatrix;

    void 
    update(const uint32_t swapChainImageIndex,
           const float swapChainImageAspectRatio);
};

#endif 