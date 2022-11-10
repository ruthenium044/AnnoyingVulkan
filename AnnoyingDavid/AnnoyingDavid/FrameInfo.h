#pragma once
#include "Buffer.h"
#include "Camera.h"
#include "GameObj.h"

namespace svk {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera &camera;
        VkDescriptorSet globalDescriptorSet;
        GameObj::Map &gameObjs;
    };
}
