#pragma once
#include "Buffer.h"
#include "Camera.h"
#include "Descriptors.h"
#include "GameObj.h"

namespace svk {
    struct PointLight {
        glm::vec4 position{};
        glm::vec4 color{};
    };
    struct GlobalUbo {
        glm::mat4 projection{1.0f};
        glm::mat4 view{1.0f};
        glm::mat4 inverseView{1.0f};
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
        DescriptorPool &frameDescriptorPool;
        GameObj::Map &gameObjs;
    };
}
