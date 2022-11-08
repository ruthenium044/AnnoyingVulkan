#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec3 fragPosWorld;

layout(push_constant) uniform Push {
    mat4 transform;
    mat4 normalMatrix;
} push;

void main() {
    //vec4 positionWorld = push.modelMatrix * vec4(inPosition, 1.0);
    gl_Position = push.transform * vec4(inPosition, 1.0);
    //gl_Position = ubo.projection * ubo.view * positionWorld;
    fragNormalWorld = normalize(mat3(push.normalMatrix) * inNormal);
    //fragPosWorld = positionWorld.xyz;
    fragColor = inColor;
}