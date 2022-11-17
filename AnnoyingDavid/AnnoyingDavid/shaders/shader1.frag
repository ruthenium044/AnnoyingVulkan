#version 450

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
    vec4 ambientColor; //w str
    PointLight pointLights[10];
    int numLights;
} ubo;

layout (set = 1, binding = 1) uniform sampler2D diffuseMap;
//layout (set = 1, binding = 2) uniform sampler2D specMap;

vec3 lightDirection = {5.0f, -5.0f, -5.0f};
float specIntensity = 0.3f;
float diffIntensity = 0.5f;

vec3 materialAmbient = { 1.0f, 0.5f, 0.31f};
vec3 materialDiffuse = { 1.0f, 0.5f, 0.31f};
vec3 materialSpecular = { 0.5f, 0.5f, 0.5f};
float materialShininess = 32.0f;

vec3 CalculateDirectional(vec3 texture, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(lightDirection);
    vec3 intensity = ubo.ambientColor.xyz * ubo.ambientColor.w;
    vec3 ambient = intensity * texture;

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    diff = clamp(diff, 0.0f, 1.0f);
    vec3 diffuse = diffIntensity * diff * texture;

    //specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specular = specIntensity * spec * materialSpecular * texture;

    return (ambient + diffuse);
}

vec3 CalculatePointLight(PointLight light, vec3 texture, vec3 normal, vec3 viewDir)
{
    //diffuse
    vec3 lightDir = (light.position.xyz - fragPosWorld);
    float attenuation = 1.0 / dot(lightDir, lightDir);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;
    lightDir = normalize(lightDir);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffIntensity * diff * texture;

    //specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specular = specIntensity * (spec * materialSpecular) * texture;
    
    vec3 ambient = intensity * texture;
    return (ambient + diffuse + specular);
}

void main() {
    
    vec3 texture = texture(diffuseMap, fragUv).xyz;
    vec3 norm = normalize(fragNormalWorld);
    vec3 cameraPosWorld = ubo.view[3].xyz;
    vec3 viewDir = normalize(cameraPosWorld - fragPosWorld);
    
    vec3 result = vec3(0.0f);
    result = CalculateDirectional(texture, norm, viewDir);
    for (int i = 0; i < ubo.numLights; i++) {
        //result += CalculatePointLight(ubo.pointLights[i], texture, norm, viewDir);
        //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);  
    }
    outColor = vec4(result, 1.0);
}   