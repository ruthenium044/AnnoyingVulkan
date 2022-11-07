#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(push_constant) uniform Push {
    mat4 transform;
    vec3 color;
} push;

void main() {
    gl_Position = push.transform * vec4(position, 1.0);
}

//#version 450
//
//layout(binding = 0) uniform UniformBufferObject {
//    mat4 model;
//    mat4 view;
//    mat4 proj;
//} ubo;
//
//layout(push_constant) uniform Push {
//    vec2 offset;
//    vec3 color;
//} push;
//
//layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec3 inColor;
//layout(location = 2) in vec2 inTexCoord;
//layout(location = 3) in vec3 inNormal;
//
//layout(location = 0) out vec3 fragColor;
//layout(location = 1) out vec2 fragTexCoord;
//layout(location = 2) out vec3 fragNormal;
//layout(location = 3) out vec3 fragPos;
//
//layout(location = 4) out vec3 viewPos;
//
//void main() {
//    vec4 posWorld = ubo.model * vec4(inPosition + push.offset, 1.0);
//    gl_Position = ubo.proj * ubo.view * posWorld;
//    fragNormal = normalize(mat3(transpose(inverse(ubo.model))) * inNormal);
//    fragPos = posWorld.xyz;
//    
//    viewPos = vec3(transpose(ubo.view));
//    
//    fragColor = inColor;
//    fragTexCoord = inTexCoord;
//}