#include "SimpleRenderSystem.h"

#include <stdexcept>

namespace svk {

    struct SimplePushConstant
    {
        alignas(16) glm::mat4 modelMatrix{1.0f};
        alignas(16) glm::mat4 normalMatrix{1.0f};
    };
    
    SimpleRenderSystem::SimpleRenderSystem(Device& dev, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout): device(dev) {
        createPipelineLayer(globalSetLayout);
        createPipeline(renderPass);
    }
    
    SimpleRenderSystem::~SimpleRenderSystem() { vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr); }

    void SimpleRenderSystem::renderGameObjs(FrameInfo &frameInfo, std::vector<GameObj>& gameObjs) {
        pipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
            0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);
       
        for(auto& obj: gameObjs) {
            SimplePushConstant push{};
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();

            vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_ALL, 0,
                              sizeof(SimplePushConstant), &push);
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }

    void SimpleRenderSystem::createPipelineLayer(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstant);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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
