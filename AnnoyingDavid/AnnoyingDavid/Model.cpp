#include "Model.h"

namespace svk
{
    VkVertexInputBindingDescription Model::Vertex::getBindingDescriptions()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 4> Model::Vertex::getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
        attributeDescriptions[0] = {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)};
        attributeDescriptions[1] = {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)};
        attributeDescriptions[2] = {2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord)};
        attributeDescriptions[3] = {3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)};
        return attributeDescriptions;
    }

    Model::Model(Device& dev, const std::vector<Vertex>& vertices)
    : device{dev}
    {
        createVertexBuffers(vertices);
    }

    Model::~Model()
    {
        vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
        vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);
    }

    void Model::draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }

    void Model::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] {vertexBuffer};
        VkDeviceSize offsets[] {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void Model::createVertexBuffers(const std::vector<Vertex>& vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        
        device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer, vertexBufferMemory);

        void* data;
        vkMapMemory(device.getDevice(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(device.getDevice(), vertexBufferMemory);

        //device. createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        //    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
        //device.copyBuffer(vertexBuffer, vertexBuffer, bufferSize);
        //vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
        //vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);
    }
}
