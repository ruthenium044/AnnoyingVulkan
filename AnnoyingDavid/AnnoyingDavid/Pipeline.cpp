﻿#include "Pipeline.h"

#include <fstream>
#include <vulkan/vulkan_core.h>

namespace svk
{
	Pipeline::Pipeline(Device &device, const std::string& vertFilepath,
		const std::string& fragFilepath, const PipelineConfigInfo& configInfo) : device(device)
	{
		createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
	}

	Pipeline::~Pipeline()
	{
		vkDestroyShaderModule(device.getDevice(), vertShaderModule, nullptr);
		vkDestroyShaderModule(device.getDevice(), fragShaderModule, nullptr);
		vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);
	}

	std::vector<char> Pipeline::readFile(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);
	
        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
    
	void Pipeline::createGraphicsPipeline(const std::string& vertFilepath,
		const std::string& fragFilepath, const PipelineConfigInfo& configInfo)
    {
        auto vertShaderCode = readFile(vertFilepath);
        auto fragShaderCode = readFile(fragFilepath);

        createShaderModule(vertShaderCode, &vertShaderModule);
        createShaderModule(fragShaderCode, &fragShaderModule);
		
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
		
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
		
        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
		
		//auto bindingDescription = Vertex::getBindingDescription();
		//auto attributeDescriptions = Vertex::getAttributeDescriptions();
	    //
        //VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        //vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		//vertexInputInfo.vertexBindingDescriptionCount = 1;
		//vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		//vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		//vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		//
        //std::vector<VkDynamicState> dynamicStates = {
        //    VK_DYNAMIC_STATE_VIEWPORT,
        //    VK_DYNAMIC_STATE_SCISSOR
        //};
        //VkPipelineDynamicStateCreateInfo dynamicState{};
        //dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        //dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        //dynamicState.pDynamicStates = dynamicStates.data();
		
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &configInfo.viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &configInfo.scissor;
		
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = nullptr;//&vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &configInfo.rasterizer;
		pipelineInfo.pMultisampleState = &configInfo.multisampling;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencil;
		pipelineInfo.pColorBlendState = &configInfo.colorBlending;
		pipelineInfo.pDynamicState = nullptr;//&dynamicState;
		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;
		
		if (vkCreateGraphicsPipelines(device.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo,
			nullptr, &graphicsPipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline!");
		}
		
        vkDestroyShaderModule(device.getDevice(), fragShaderModule, nullptr);
        vkDestroyShaderModule(device.getDevice(), vertShaderModule, nullptr);
    }

	void Pipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		
		if (vkCreateShaderModule(device.getDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module!");
		}
	}
	
	PipelineConfigInfo Pipeline::defaultPipelineConfigInfor(uint32_t width, uint32_t height)
	{
		PipelineConfigInfo configInfo{};

		configInfo.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;
		
		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = (float) width;//swapChainExtent.width;
		configInfo.viewport.height = (float) height;//swapChainExtent.height;
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;
	
		configInfo.scissor.offset = {0, 0};
		configInfo.scissor.extent = {width, height};//swapChainExtent;
		
        configInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        configInfo.rasterizer.depthClampEnable = VK_FALSE;
        configInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
        configInfo.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        configInfo.rasterizer.lineWidth = 1.0f;
        configInfo.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        configInfo.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        configInfo.rasterizer.depthBiasEnable = VK_FALSE;
		configInfo.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		configInfo.rasterizer.depthBiasClamp = 0.0f; // Optional
		configInfo.rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
		
        configInfo.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        configInfo.multisampling.sampleShadingEnable = VK_FALSE;
        //configInfo.multisampling.rasterizationSamples = msaaSamples;
		configInfo.multisampling.minSampleShading = 1.0f; // Optional
		configInfo.multisampling.pSampleMask = nullptr; // Optional
		configInfo.multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		configInfo.multisampling.alphaToOneEnable = VK_FALSE; // Optional
		configInfo.multisampling.sampleShadingEnable = VK_TRUE; // enable sample shading in the pipeline
		configInfo.multisampling.minSampleShading = 0.2f; // min fraction for sample shading; closer to one is smoother
		
		configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; //Optional
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; //Optional
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optiona
		
		configInfo.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlending.logicOpEnable = VK_FALSE;
		configInfo.colorBlending.logicOp = VK_LOGIC_OP_COPY;
		configInfo.colorBlending.attachmentCount = 1;
		configInfo.colorBlending.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlending.blendConstants[0] = 0.0f;
		configInfo.colorBlending.blendConstants[1] = 0.0f;
		configInfo.colorBlending.blendConstants[2] = 0.0f;
		configInfo.colorBlending.blendConstants[3] = 0.0f;

		configInfo.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencil.depthTestEnable = VK_TRUE;
		configInfo.depthStencil.depthWriteEnable = VK_TRUE;
		configInfo.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencil.stencilTestEnable = VK_FALSE;
		configInfo.depthStencil.minDepthBounds = 0.0f; // Optional
		configInfo.depthStencil.maxDepthBounds = 1.0f; // Optional
		configInfo.depthStencil.front = {}; // Optional
		configInfo.depthStencil.back = {}; // Optional

		return configInfo;
	}
}


