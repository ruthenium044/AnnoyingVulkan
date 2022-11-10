#pragma once
#include "Camera.h"
#include "Device.h"
#include "FrameInfo.h"
#include "GameObj.h"
#include "Pipeline.h"

namespace svk {
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(Device& dev, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void renderGameObjs(FrameInfo &frameInfo, std::vector<GameObj>& gameObjs);
    private:
        
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);
        
        Device& device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout{};
    };
}
