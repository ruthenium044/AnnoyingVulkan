#pragma once
#include "Buffer.h"
#include "Camera.h"

namespace svk {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
}
