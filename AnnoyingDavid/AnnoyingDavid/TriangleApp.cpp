#include "TriangleApp.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <chrono>

#include "MovementController.h"
#include "SimpleRenderSystem.h"

namespace svk
{
    struct GlobalUbo {
        alignas(16) glm::mat4 projection{1.0f};

        alignas(16) glm::vec4 ambientColor{0.1f, 0.1f, 0.1f, 0.02f};
        alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, 1.0f});
        alignas(16) glm::vec3 lightPosition{1.0f, 2.0f, -1.0f};
        alignas(16) glm::vec4 lightColor{0.0f, 0.4f, 0.0f, 1.0f};
    };
    
    TriangleApp::TriangleApp() {
        globalPool = DescriptorPool::Builder(device).setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT).build();
        loadGameObjs();
    }
    
    TriangleApp::~TriangleApp() { }

    void TriangleApp::run() {
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); ++i) {
            uboBuffers[i] = std::make_unique<Buffer>(device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout = DescriptorSetLayout::Builder(device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); ++i) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
        }
        
        SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        Camera camera{};

        auto viewerObject = GameObj::createGameObj();
        viewerObject.transform.translation = {0.0f, -3.0f, -5.0f};
        viewerObject.transform.rotation = {glm::radians(-30.0f), 0.0f, 0.0f};
        MovementController cameraController{};
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        while (isRunning) {
            SDL_Event e;
            cameraController.refresh();
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                case SDL_WINDOWEVENT: {
                    switch (e.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED: 
                        //recreateSwapChain();
                        break;
                    case SDL_WINDOWEVENT_CLOSE: 
                        e.type = SDL_QUIT;
                        SDL_PushEvent(&e);
                        break;
                    }
                    break;
                }
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_KEYDOWN: 
                    cameraController.updateKey(e.key.keysym.scancode, true);
                    break;
                case SDL_KEYUP: 
                    cameraController.updateKey(e.key.keysym.scancode, false);
                    break;
                }
            }
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;
            frameTime = glm::min(frameTime, 0.5f);

            cameraController.update(frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
                
            float aspect = renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1, 100);
            
            if (auto commandBuffer = renderer.beginFrame()) {
                int frameIndex = renderer.getFrameIndex();
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera,
                    globalDescriptorSets[frameIndex], gameObjs};
                
                //update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                
                //render
                renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjs(frameInfo);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(device.getDevice()); }
    
    void TriangleApp::loadGameObjs() {
        std::shared_ptr<Model> model = Model::createModelFromFile(device, "models/skull/skull.obj");
        auto skull1 = GameObj::createGameObj();
        skull1.model = model;
        skull1.color = {0, 0, 0};
        skull1.transform.rotation = {glm::radians(90.0f), 0.0f, 0.0f};
        skull1.transform.translation = {0.0f, 0.0f, 0.0f};
        skull1.transform.scale = {0.05f, 0.05f, 0.05f};
        gameObjs.emplace(skull1.getId(), std::move(skull1));

        model = Model::createModelFromFile(device, "models/quad.obj");
        auto plane = GameObj::createGameObj();
        plane.model = model;
        plane.color = {0.5, 0.5, 0};
        plane.transform.translation = {0.0f, 0.0f, 0.0f};
        plane.transform.scale = {2.0f, 1.0f, 2.0f};
        gameObjs.emplace(plane.getId(), std::move(plane));
    }
}
