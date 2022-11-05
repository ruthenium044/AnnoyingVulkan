#include "TriangleApp.h"

#include <array>
#include <stdexcept>
#include <unordered_map>

#include "tiny_obj_loader.h"

namespace svk
{
    TriangleApp::TriangleApp()
    {
        LoadModels("models/skull.obj");
        createPipelineLayer();
        recreateSwapChain();
        createCommandBuffers();
    }

    TriangleApp::~TriangleApp()
    {
        vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
    }

    void TriangleApp::run()
    {
        mainLoop();
    }

    void TriangleApp::LoadModels(const char* filepath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath))
        {
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};
                if (index.vertex_index >= 0)
                {
                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]};
                }
                if (index.normal_index >= 0)
                {
                    vertex.normal = {
                        attrib.normals[3 * index.vertex_index + 0],
                        attrib.normals[3 * index.vertex_index + 1],
                        attrib.normals[3 * index.vertex_index + 2]};
                }
                if (index.texcoord_index >= 0)
                {
                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
                }
                vertex.color = {1.0f, 1.0f, 1.0f};
			
                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

    void TriangleApp::createPipelineLayer()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
	    
        if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void TriangleApp::createPipeline()
    {
        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfor(pipelineConfig);
        pipelineConfig.renderPass = swapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(device, "shaders/shader1.vert.spv",
            "shaders/shader1.frag.spv", pipelineConfig);
    }

    void TriangleApp::createCommandBuffers()
    {
        commandBuffers.resize(swapChain->imageCount());
	
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
	
        if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void TriangleApp::freeCommandBuffers()
    {
        vkFreeCommandBuffers(device.getDevice(), device.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void TriangleApp::drawFrame()
    {
        uint32_t imageIndex;
        auto result = swapChain->acquireNextImage(&imageIndex);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        recordCommandBuffer(imageIndex);
        result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized())
        {
            window.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    void TriangleApp::recreateSwapChain()
    {
        auto extent = window.getExtent();
        
        while (extent.width == 0 || extent.height == 0)
        {
            SDL_Event e;
            e.type = SDL_WINDOWEVENT;
            e.window.event = SDL_WINDOWEVENT_MAXIMIZED;
            SDL_WaitEvent(&e);
        }

        vkDeviceWaitIdle(device.getDevice());
        if (swapChain == nullptr)
        {
            swapChain = std::make_unique<SwapChain>(device, extent);
        }
        else
        {
            swapChain = std::make_unique<SwapChain>(device, extent, std::move(swapChain));
            if (swapChain->imageCount() != commandBuffers.size())
            {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }
        createPipeline();
    }

    void TriangleApp::recordCommandBuffer(uint32_t imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->getRenderPass();
        renderPassInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();
            
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.1f, 0.1f, 0.1f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChain->getSwapChainExtent();
        
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);


        
        pipeline->bind(commandBuffers[imageIndex]);
        model->bind(commandBuffers[imageIndex]);
        model->draw(commandBuffers[imageIndex]);
        //vkCmdDraw(commandBuffers[imageIndex], 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void TriangleApp::mainLoop()
    {
        while (isRunning)
        {
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                switch (e.type)
                {
                case SDL_WINDOWEVENT:
                    {
                        switch (e.window.event)
                        {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            {
                                recreateSwapChain();
                                break;
                            }
                        case SDL_WINDOWEVENT_CLOSE:
                            {
                                e.type = SDL_QUIT;
                                SDL_PushEvent(&e);
                                break;
                            }
                        }
                        break;
                    }
                case SDL_QUIT:
                    {
                        isRunning = false;
                    }
                case SDL_KEYDOWN:
                    {
                        //processInput( &e.key );
                    }
                }
            }
            drawFrame();
        }
        vkDeviceWaitIdle(device.getDevice());
    }
}
