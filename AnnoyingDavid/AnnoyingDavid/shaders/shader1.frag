#version 450

layout(binding = 1) uniform sampler2D texSampler;

//TODO GET THE LIGHT FROM LIGHT SOURCE THANKS
//layout(location = 2) uniform vec3 lightColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPos;
layout(location = 4) in vec3 viewPos;

layout(location = 0) out vec4 outColor;

//todo these shoul dbe uniform help
vec3 lightColor = vec3(0.33f, 0.3f, 0.18f);
vec3 lightPos = vec3(0.0f, -15.0f, 45.0f);
float ambientStrength = 0.1;
float specularStrength = 1.5;

void main() {
    outColor = texture(texSampler, fragTexCoord);
    
    vec3 ambient = ambientStrength * lightColor;

    //diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(fragNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    //specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(lightDir, fragNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * outColor.xyz;
    outColor = vec4(result, 1.0);
}