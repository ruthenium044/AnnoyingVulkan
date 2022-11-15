
#version 450

//layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragUv;

layout(location = 0) out vec4 outColor;

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
    vec4 ambientColor;
    PointLight pointLights[10];
    int numLights;
} ubo;

layout (set = 1, binding = 1) uniform sampler2D diffuseMap;

void main() {
    
    //diffuse
    vec3 diffuse = ubo.ambientColor.xyz * ubo.ambientColor.w;
    vec3 specular = vec3(0.0);
    vec3 surfaceNormal = normalize(fragNormalWorld);
    
    vec3 cameraPosWorld = ubo.invView[3].xyz;
    vec3 viewDir = normalize(cameraPosWorld - fragPosWorld);
    
    for (int i = 0; i < ubo.numLights; i++) {
        PointLight light = ubo.pointLights[i];
        vec3 lightDir = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(lightDir, lightDir);
        lightDir = normalize(lightDir);
        
        float cosAngIncidence = max(dot(surfaceNormal, lightDir), 0.0f);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;
        diffuse += intensity * cosAngIncidence;
        
        //specular
        vec3 halfAngle = normalize(lightDir * viewDir);
        float blinnTerm = dot(surfaceNormal, halfAngle);
        blinnTerm = clamp(blinnTerm, 0, 1);
        blinnTerm = pow(blinnTerm, 8.0f);
        specular += intensity * blinnTerm;
    }
 
    vec3 color = texture(diffuseMap, fragUv).xyz;
    vec3 result = diffuse * color + specular * color;
    outColor = vec4(result, 1.0);
}