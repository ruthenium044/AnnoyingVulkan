#pragma once
#include <string>
#include <vector>

#include "Device.h"

namespace svk
{
    struct PipelineConfigInfo
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlending;
        VkPipelineDepthStencilStateCreateInfo depthStencil;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    
    class Pipeline
    {
    public:
        Pipeline(Device &device, const std::string& vertFilepath,
            const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline &operator=(const Pipeline&) = delete;

        static PipelineConfigInfo defaultPipelineConfigInfor(uint32_t width, uint32_t height);
    private:
        static std::vector<char> readFile(const std::string& filepath);
        
        void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
        
        Device& device;
        VkPipeline graphicsPipeline{};
        VkShaderModule vertShaderModule{};
        VkShaderModule fragShaderModule{};
        
    };
    
}
