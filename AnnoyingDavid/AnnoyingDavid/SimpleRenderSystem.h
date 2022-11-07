#pragma once
#include "Camera.h"
#include "Device.h"
#include "GameObj.h"
#include "Pipeline.h"

namespace svk {
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(Device& dev, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void renderGameObjs(VkCommandBuffer commandBuffer, std::vector<GameObj>& gameObjs, const Camera& camera);
    private:
        
        void createPipelineLayer();
        void createPipeline(VkRenderPass renderPass);
        
        Device& device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout{};
    };
}
