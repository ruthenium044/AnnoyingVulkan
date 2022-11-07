#include "TriangleApp.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <array>
#include <stdexcept>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "SimpleRenderSystem.h"
#include "tiny_obj_loader.h"

namespace svk
{
    TriangleApp::TriangleApp() {
        loadGameObjs();
        //LoadModels("models/skull/skull.obj");
    }

    TriangleApp::~TriangleApp() { }

    void TriangleApp::run() { mainLoop(); }
    
    void TriangleApp::loadGameObjs() {
        std::vector<Model::Vertex> vertices;

        //todo idk where this foes
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, "models/skull/skull.obj")) { throw std::runtime_error(warn + err); }

        std::vector<uint32_t> indices;
        
        vertices.clear();
        indices.clear();
        
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Model::Vertex vertex{};
                if (index.vertex_index >= 0) {
                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                }
                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.vertex_index + 0],
                        attrib.normals[3 * index.vertex_index + 1],
                        attrib.normals[3 * index.vertex_index + 2]
                    };
                }
                if (index.texcoord_index >= 0) {
                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }
                vertex.color = {1.0f, 1.0f, 1.0f};
                vertices.push_back(vertex);
            }
        }
        
        auto model = std::make_shared<Model>(device, vertices);

        auto triangle = GameObj::createGameObj();
        triangle.model = model;
        triangle.color = {0.5, 0.5, 0};
        triangle.transform.translation = {0.0f, 0.0f, 5.0f};
        triangle.transform.scale = {0.1f, 0.1f, 0.1f};

        gameObjs.push_back(std::move(triangle));
    }

    void TriangleApp::LoadModels(const char* filepath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath)) { throw std::runtime_error(warn + err); }

        std::vector<Model::Vertex> vertices;
        std::vector<uint32_t> indices;
        
        vertices.clear();
        indices.clear();
        //::unordered_map<Model::Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Model::Vertex vertex{};
                if (index.vertex_index >= 0) {
                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                }
                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.vertex_index + 0],
                        attrib.normals[3 * index.vertex_index + 1],
                        attrib.normals[3 * index.vertex_index + 2]
                    };
                }
                if (index.texcoord_index >= 0) {
                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }
                vertex.color = {1.0f, 1.0f, 1.0f};

                //if (!uniqueVertices.contains(vertex)) {
                //    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                //    vertices.push_back(vertex);
                //}
                //indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

    void TriangleApp::mainLoop() {
        SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};
        Camera camera{};
        camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5, 0.0f, 1.0f));
        
        while (isRunning) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                float aspect = renderer.getAspectRatio();
                camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1, 50);
                
                switch (e.type) {
                case SDL_WINDOWEVENT: {
                    switch (e.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        //recreateSwapChain();
                        
                        break;
                    }
                    case SDL_WINDOWEVENT_CLOSE: {
                        e.type = SDL_QUIT;
                        SDL_PushEvent(&e);
                        break;
                    }
                    }
                    break;
                }
                case SDL_QUIT: { isRunning = false; }
                case SDL_KEYDOWN: {
                    //processInput( &e.key );
                }
                }
            }
            if (auto commandBuffer = renderer.beginFrame()) {
                renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjs(commandBuffer, gameObjs, camera);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(device.getDevice());
    }

    
    //void processInput(SDL_KeyboardEvent* key)
    //{
    //    float cameraSpeed = 1000.0f * deltaTime;
    //    switch (key->keysym.sym)
    //    {
    //    case SDLK_a:
    //        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    //        break;
    //    case SDLK_d:
    //        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    //        break;
    //    case SDLK_w:
    //        cameraPos += cameraSpeed * cameraFront;
    //        break;
    //    case SDLK_s:
    //        cameraPos -= cameraSpeed * cameraFront;
    //        break;
    //    default: ;
    //    }
    //}
}
