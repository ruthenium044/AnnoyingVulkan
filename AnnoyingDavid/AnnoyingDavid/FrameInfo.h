#pragma once
#include "Buffer.h"
#include "Camera.h"
#include "GameObj.h"

namespace svk {
    struct PointLight {
        glm::vec4 position{};
        glm::vec4 color{};
    };
    struct GlobalUbo {
        alignas(16) glm::mat4 projection{1.0f};
        alignas(16) glm::mat4 view{1.0f};
        alignas(16) glm::vec4 ambientColor{1.0f, 1.0f, 1.0f, 0.02f};
        
        PointLight pointLights[10];
        int numbLights;
    };
    
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera &camera;
        VkDescriptorSet globalDescriptorSet;
        GameObj::Map &gameObjs;
    };
}
