#pragma once
#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <array>
#include <glm/glm.hpp>

namespace svk
{
    class Model
    {
    public:

        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 color;
            glm::vec2 texCoord;
            glm::vec3 normal;

            static VkVertexInputBindingDescription getBindingDescriptions();
            static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();

            bool operator==(const Vertex& other) const
            {
                return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
            }
        };
        
        Model(Device &dev, const std::vector<Vertex> &vertices);
        ~Model();

        Model(const Model &) = delete;
        Model &operator=(const Model &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);
        
    private:

        void createVertexBuffers(const std::vector<Vertex> &vertices);
        
        Device device;
        VkBuffer vertexBuffer{};
        VkDeviceMemory vertexBufferMemory{};
        uint32_t vertexCount{};
        
    
    };
}
