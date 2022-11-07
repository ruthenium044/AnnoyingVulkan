#pragma once

#include <memory>

#include "Model.h"
#include "SwapChain.h"
#include "Window.h"

namespace svk {
    class Renderer {
    public:
        Renderer(Window& win, Device& dev);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
        float getAspectRatio() const { return swapChain->extentAspectRatio(); };
        bool isFrameInProgress() const{ return isFrameStarted; }
        VkCommandBuffer getCurrentCommandBuffer() const {
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();
       
        bool isRunning = true;

        Window& window;
        Device& device;
        std::unique_ptr<SwapChain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex;
        bool isFrameStarted;
        
        void mainLoop();
    };
}