#include "TriangleApp.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <chrono>

#include "MovementController.h"
#include "PointRenderingSystem.h"
#include "SimpleRenderSystem.h"

namespace svk
{
    TriangleApp::TriangleApp() {
        globalPool = DescriptorPool::Builder(device).setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT).build();
        // build frame descriptor pools
        framePools.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        auto framePoolBuilder = DescriptorPool::Builder(device)
                                    .setMaxSets(1000)
                                    .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
                                    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
                                    .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
        for (int i = 0; i < framePools.size(); i++) {
            framePools[i] = framePoolBuilder.build();
        }
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
        PointRenderingSystem pointRenderSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        Camera camera{};

        auto& viewerObject = gameObjectManager.createGameObject();
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
                        //todo recreate swap chain heere
                        // Block until it is back
                        // recreate swapchain
                        // recreate depth resource
                        // recreate frame buffers
                        //renderer.recreateSwapChain();
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
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100);
            
            if (auto commandBuffer = renderer.beginFrame()) {
                int frameIndex = renderer.getFrameIndex();
                framePools[frameIndex]->resetPool();
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera,
                    globalDescriptorSets[frameIndex], *framePools[frameIndex],
                gameObjectManager.gameObjects};

                //todo how to correctly update
                //gameObjectManager.gameObjects.at(1).transform.rotation =
                //    {glm::radians(45.0f * frameTime ), 0.0f, 0.0f};
                //
                //auto rotation = glm::rotate(glm::mat4(1.0f), frameInfo.frameTime,
                //    {0.0f, -1.0f, 0.0f});
                //auto& obj = gameObjectManager.gameObjects.at(gameObjectManager.usedIds.at(0));
                //obj.transform.translation = glm::vec3(rotation * glm::vec4(obj.transform.translation, 1.0f));
                
                //update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                pointRenderSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                gameObjectManager.updateBuffer(frameIndex);
                
                //render
                renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjs(frameInfo);
                pointRenderSystem.render(frameInfo);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(device.getDevice());
    }
    
    void TriangleApp::loadGameObjs() {
        
        std::shared_ptr model = Model::createModelFromFile(device, "models/skull/skull.obj");
        std::shared_ptr texture = Texture::createTextureFromFile(device, "models/skull/skull.jpg");
        std::shared_ptr specTexture = Texture::createTextureFromFile(device, "models/skull/skullSpec.png");
        glm::vec3 scale = {0.06f, 0.06f, 0.06f};

        int numPerRow = 9;
        int numRows = numPerRow - 2;
        float angle = glm::two_pi<float>() / static_cast<float>(numPerRow);
        float radiusDelta = 0.3f;
        float radius = numRows * radiusDelta + radiusDelta;
       
        std::vector<TransfromComponent> skullTransforms = {};
        for (int j = 0; j < numRows; j ++) {
            for (int i = 0; i < numPerRow; ++i) {
                
                
                skullTransforms.push_back({
                    {cos(i * angle) * (radius - j * radiusDelta), -1.3f * j , sin(i * angle) * (radius - j * radiusDelta) }, scale,
                    {glm::radians(90.0f - 5.0f * j), glm::radians(-90.0f) + (numPerRow - i) * angle, 0.0f}});
            }
            numPerRow--;
            angle = glm::two_pi<float>() / static_cast<float>(numPerRow);
        }
        
        for (auto& transform : skullTransforms) {
            auto& skull1 = gameObjectManager.createGameObject();
            skull1.model = model;
            skull1.diffuseMap = texture;
            //skull1.specularMap = specTexture;
            skull1.transform = transform;
        }
        
        model = Model::createModelFromFile(device, "models/quad.obj");
        texture = Texture::createTextureFromFile(device, "textures/bg.JPG");
        scale = {6.0f, 6.0f, 6.0f};
        std::vector<TransfromComponent> planeTransforms = {
            {{0.0f, 0.0f, 0.0f}, scale, {glm::radians(0.0f), 0.0f, 0.0f}},
            {{0.0f, -scale.y, scale.z}, scale, {glm::radians(90.0f), 0.0f, 0.0f}},
            {{0.0f, -scale.y * 2, 0.0f}, scale, {glm::radians(180.0f), 0.0f, 0.0f}},
            {{scale.x, -scale.y, 0.0f}, scale, {glm::radians(90.0f), glm::radians(90.0f), 0.0f}},
            {{-scale.x, -scale.y, 0.0f}, scale, {glm::radians(90.0f), glm::radians(-90.0f), 0.0f}},
            {{0.0f, -scale.y, -scale.z}, scale, {glm::radians(90.0f), glm::radians(180.0f), 0.0f}},
        };

        for (auto& transform : planeTransforms) {
            auto& plane = gameObjectManager.createGameObject();
            plane.model = model;
            plane.diffuseMap = texture;
            plane.transform = transform;
        }
        
        std::vector<glm::vec3> lightColors{
          {1.0f, 0.1f, 0.1f},
          {1.0f, 1.0f, 0.1f},
          {0.1f, 1.0f, 0.1f},
          {0.1f, 1.0f, 1.0f},
          {0.1f, 0.1f, 1.0f},
          {1.0f, 0.1f, 1.0f},
        };

        for (int i = 0; i < lightColors.size(); ++i) {
            auto& pointLight = gameObjectManager.makePointLight(0.1f);
            pointLight.color = lightColors[i];
            glm::vec3 rotation = {glm::radians(90.0f), 0.0f, 0.0f};
            auto rotateLight = glm::rotate(glm::mat4(1.0f), (i * glm::two_pi<float>()) / lightColors.size(), {0.0f, -1.0f, 0.0f});
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-2.0f, -3.0f, -2.0f, 1.0f));
        }
    }
}
