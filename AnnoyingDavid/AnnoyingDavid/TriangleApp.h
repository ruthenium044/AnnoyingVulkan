#pragma once
#include <memory>

#include "Pipeline.h"
#include "SwapChain.h"
#include "Window.h"

namespace svk
{
    class TriangleApp
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        TriangleApp();
        ~TriangleApp();

        TriangleApp(const TriangleApp &) = delete;
        TriangleApp &operator=(const TriangleApp &) = delete;
        
        void run();
    private:

        void createPipelineLayer();
        void createPipeline();
        void createCommandBuffers() {};
        void drawFrame() {};
        
        bool isRunning = true;
        
        Window window{WIDTH, HEIGHT};
        Device device{window};
        SwapChain swapChain{device, window.getExtent()};

        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;

        void mainLoop();
    };
}
