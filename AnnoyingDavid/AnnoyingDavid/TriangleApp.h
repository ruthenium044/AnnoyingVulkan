#pragma once
#include <memory>

#include "Model.h"
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

        void LoadModels(const char* filepath);
        
        void createPipelineLayer();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(uint32_t imageIndex);
        
        bool isRunning = true;
        
        Window window{WIDTH, HEIGHT};
        Device device{window};
        std::unique_ptr<SwapChain> swapChain;

        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<Model> model;

        void mainLoop();
    };
}
