#include "MatrixUBO.h"

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std::chrono;

void 
MatrixUBO::update(const size_t swapChainImageIndex,
                  const float swapChainImageAspectRatio) {
    // Calculate time in seconds since rendering has started with floating point accuracy.
    static time_point<high_resolution_clock> startTime = high_resolution_clock::now();
    const time_point<high_resolution_clock> currentTime = high_resolution_clock::now();
    const float timeInSeconds = duration<float, seconds::period>(currentTime - startTime).count();

    // Model rotation will be a simple rotation around the
    // Z-axis using the timeInSeconds variable (to rotate 90 degrees per second).
    const mat4 identityMat4 = mat4(1.0f);
    const float angle = timeInSeconds * radians(90.0f);
    const vec3 axis(0.0f, 0.0f, 1.0f);
    mModelMatrix = rotate(identityMat4,
                          angle,
                          axis);

    // For the view transformation, we will look at the geometry from
    // above at a 45 degree angle.
    const vec3 eyeVec(2.0f, 2.0f, 2.0f);
    const vec3 centerVec(0.0f, 0.0f, 0.0f);
    const vec3 upVec(0.0f, 0.0f, 1.0f);
    mViewMatrix = lookAt(eyeVec, 
                         centerVec,
                         upVec);

    // We use a perspective projection with a 45 degree vertical
    // field of view. 
    constexpr float fieldOfViewY = radians(45.0f);
    const float nearPlaneZ = 0.1f;
    const float farPlaneZ = 10.0f;
    mProjectionMatrix = perspective(fieldOfViewY,
                                    swapChainImageAspectRatio,
                                    nearPlaneZ,
                                    farPlaneZ);
                                    
    // GLM was originally designed for OpenGL, where the Y coordinate
    // of the clip coordinates is inverted.
    // The easiest way to compensate for that is to flip the sign
    // of the scaling factor of the Y axis in the projection matrix.
    // If we do not do this, then the image will be rendered 
    // upside down.
    mProjectionMatrix[1][1] *= -1;
}