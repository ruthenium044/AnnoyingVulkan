#include "TriangleApp.h"

#include <stdexcept>

namespace svk
{
    TriangleApp::TriangleApp()
    {
        createPipelineLayer();
        createPipeline();
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
        auto pipelineConfig = Pipeline::defaultPipelineConfigInfor(swapChain.width(), swapChain.height());
        pipelineConfig.renderPass = swapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(device, "shaders/shader1.vert.spv",
            "shaders/shader1.frag.spv", pipelineConfig);
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
                                //recreateSwapChain();
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
            //drawFrame();
        }
        //vkDeviceWaitIdle(device);
    }
}
