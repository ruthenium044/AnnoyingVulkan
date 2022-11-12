
#version 450

//layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

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
    vec4 ambientColor;
    PointLight pointLights[10];
    int numLights;
} ubo;

void main() {
    //outColor = texture(texSampler, fragTexCoord);
    
    //vec3 ambient = ambientStrength * lightColor;
    
    //diffuse
    vec3 diffuse = ubo.ambientColor.xyz * ubo.ambientColor.w;
    vec3 surfaceNormal = normalize(fragNormalWorld);
    
    for (int i = 0; i < ubo.numLights; i++) {
        PointLight light = ubo.pointLights[i];
        vec3 lightDir = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(lightDir, lightDir);
        float cosAngIncidence = max(dot(surfaceNormal, normalize(lightDir)), 0.0f);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;
        diffuse += intensity * cosAngIncidence;
    }
    
    //vec3 ambientColor = ubo.ambientColor.xyz * ubo.ambientColor.w;
    
    //specular
    //vec3 viewDir = normalize(ubo.lightPosition - fragPosWorld);
    //vec3 reflectDir = reflect(lightDir, fragNormalWorld);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    //vec3 specular = 0.2f * spec * lightColor;

    vec3 result = (diffuse) * fragColor.xyz;
    outColor = vec4(result, 1.0);
}