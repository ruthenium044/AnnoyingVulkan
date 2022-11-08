#include "TriangleApp.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <chrono>

#include "MovementController.h"
#include "SimpleRenderSystem.h"

namespace svk
{
    TriangleApp::TriangleApp() { loadGameObjs(); }
    
    TriangleApp::~TriangleApp() { }

    void TriangleApp::run() { mainLoop(); }
    
    void TriangleApp::loadGameObjs() {
        std::shared_ptr<Model> model = Model::createModelFromFile(device, "models/skull/skull.obj");

        auto triangle = GameObj::createGameObj();
        triangle.model = model;
        triangle.color = {0.5, 0.5, 0};
        triangle.transform.translation = {0.0f, 0.0f, 0.0f};
        triangle.transform.scale = {0.1f, 0.1f, 0.1f};

        gameObjs.push_back(std::move(triangle));
    }

    void TriangleApp::mainLoop() {
        SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};
        Camera camera{};

        auto viewerObject = GameObj::createGameObj();
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
                renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjs(commandBuffer, gameObjs, camera);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(device.getDevice());
    }
}
