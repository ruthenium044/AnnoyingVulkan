﻿#pragma once
#include "Device.h"
#include "Utils.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <string>

namespace svk {
    class Model {
    public:
        struct Vertex {
            glm::vec3 pos;
            glm::vec3 color;
            glm::vec2 texCoord;
            glm::vec3 normal;

            static VkVertexInputBindingDescription getBindingDescriptions();
            static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();

            bool operator==(const Vertex& other) const {
                return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
            }
        };

        struct Builder {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            void loadModel(const std::string &filepath);
        };

        Model(Device& dev, const Builder &builder);
        ~Model();

        static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string &filepath); 
        
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);

        Device& device;
        
        VkBuffer vertexBuffer{};
        VkDeviceMemory vertexBufferMemory{};
        uint32_t vertexCount{};

        bool hasIndexBuffer = false;
        VkBuffer indexBuffer{};
        VkDeviceMemory indexBufferMemory{};
        uint32_t indexCount{};
    };
}
