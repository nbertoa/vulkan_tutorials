#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModelMatrix;
	mat4 mViewMatrix;
	mat4 mProjectionMatrix;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.mProjectionMatrix * ubo.mViewMatrix * ubo.mModelMatrix * vec4(inPosition, 1.0);
	
	fragTexCoord = inTexCoord;
}