
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

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec4 ambientColor;
    vec3 lightDirection;
    vec3 lightPosition;
    vec4 lightColor;
} ubo;

void main() {
    //outColor = texture(texSampler, fragTexCoord);
    
    //vec3 ambient = ambientStrength * lightColor;
    
    //diffuse
    vec3 lightDir = normalize(ubo.lightPosition - fragPosWorld);
    float attenuation = 1.0 / dot(lightDir, lightDir);
    
    vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
    vec3 ambientColor = ubo.ambientColor.xyz * ubo.ambientColor.w;
    
    float diff = max(dot(normalize(fragNormalWorld), lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    //specular
    vec3 viewDir = normalize(ubo.lightPosition - fragPosWorld);
    vec3 reflectDir = reflect(lightDir, fragNormalWorld);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = 0.2f * spec * lightColor;

    vec3 result = (diffuse + ambientColor) * fragColor.xyz;
    outColor = vec4(result, 1.0);
}