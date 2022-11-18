#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 fragUv;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 lightColor;
    PointLight pointLights[10];
    int numLights;
} ubo;

layout(set = 1, binding = 0) uniform GameObjectBufferData {
    mat4 modelMatrix;
    mat4 normalMatrix;
} gameObject;

void main() {
    vec4 positionWorld = gameObject.modelMatrix * vec4(inPosition, 1.0);
    gl_Position = ubo.projection * ubo.view * positionWorld;
    
    fragNormalWorld = normalize(mat3(gameObject.normalMatrix) * inNormal);
    fragPosWorld = positionWorld.xyz;
    fragColor = inColor;
    fragUv = inTexCoord;
}