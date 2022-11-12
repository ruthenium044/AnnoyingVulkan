#pragma once
#include <memory>

#include "FrameInfo.h"
#include "Pipeline.h"

namespace svk {
    struct FrameInfo;

    class PointRenderingSystem {
    public:
        PointRenderingSystem(Device& dev, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointRenderingSystem();

        PointRenderingSystem(const PointRenderingSystem&) = delete;
        PointRenderingSystem& operator=(const PointRenderingSystem&) = delete;

        void update(FrameInfo &frameInfo, GlobalUbo &ubo);
        void render(FrameInfo &frameInfo);
    private:
        
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);
        
        Device& device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout{};
    };
}
