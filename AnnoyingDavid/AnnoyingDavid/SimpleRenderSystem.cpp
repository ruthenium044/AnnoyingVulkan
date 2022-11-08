#include "SimpleRenderSystem.h"

#include <stdexcept>

namespace svk {

    struct SimplePushConstant
    {
        alignas(16) glm::mat4 transform{1.0f};
        alignas(16) glm::mat4 normalMatrix{1.0f};
    };
    
    SimpleRenderSystem::SimpleRenderSystem(Device& dev, VkRenderPass renderPass): device(dev) {
        createPipelineLayer();
        createPipeline(renderPass);
    }
    
    SimpleRenderSystem::~SimpleRenderSystem() { vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr); }

    void SimpleRenderSystem::renderGameObjs(VkCommandBuffer commandBuffer, std::vector<GameObj>& gameObjs, const Camera& camera) {
        pipeline->bind(commandBuffer);

        auto proejctionView = camera.getProjection() * camera.getView();
        for(auto& obj: gameObjs) {
            SimplePushConstant push{};
            auto modelMatrix = obj.transform.mat4();
            push.transform = proejctionView * modelMatrix;
            push.normalMatrix = obj.transform.normalMatrix();

            vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_ALL, 0,
                              sizeof(SimplePushConstant), &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }

    void SimpleRenderSystem::createPipelineLayer() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstant);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }
    
    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfor(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(device, "shaders/vert.spv",
                                              "shaders/frag.spv", pipelineConfig);
    
    }
}
