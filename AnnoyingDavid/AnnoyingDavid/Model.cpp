#include "Model.h"
                                       
#define TINYOBJLOADER_IMPLEMENTATION   
#include "tiny_obj_loader.h"           

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std {
    template <>
    struct hash<svk::Model::Vertex> {
        size_t operator()(svk::Model::Vertex const &vertex) const {
            size_t seed = 0;
            svk::hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.texCoord);
            return seed;
        }
    };
};

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

    Model::Model(Device& dev, const Builder &builder)
    : device(dev)
    {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    Model::~Model()
    {
        vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
        vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);
        if (hasIndexBuffer) {
            vkDestroyBuffer(device.getDevice(), indexBuffer, nullptr);
            vkFreeMemory(device.getDevice(), indexBufferMemory, nullptr);
        }
    }

    std::unique_ptr<Model> Model::createModelFromFile(Device& device, const std::string& filepath) {
        Builder builder;
        builder.loadModel(filepath);
        return std::make_unique<Model>(device, builder);
    }

    void Model::draw(VkCommandBuffer commandBuffer)
    {
        if (hasIndexBuffer) {
            vkCmdDraw(commandBuffer, indexCount, 1, 0, 0);
        }
        else {
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }

    void Model::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] {vertexBuffer};
        VkDeviceSize offsets[] {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if (hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void Model::createVertexBuffers(const std::vector<Vertex>& vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingbufferMemory;
        device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingbufferMemory);

        void* data;
        vkMapMemory(device.getDevice(), stagingbufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(device.getDevice(), stagingbufferMemory);

        device.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer, vertexBufferMemory);
        device.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
        vkFreeMemory(device.getDevice(), stagingbufferMemory, nullptr);
    }

    void Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        if (!hasIndexBuffer) {
            return;
        }

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
        
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingbufferMemory;
        device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingbufferMemory);

        void* data;
        vkMapMemory(device.getDevice(), stagingbufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t) bufferSize);
        vkUnmapMemory(device.getDevice(), stagingbufferMemory);

        device.createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer, indexBufferMemory);
        device.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
        vkFreeMemory(device.getDevice(), stagingbufferMemory, nullptr);
    }

    void Model::Builder::loadModel(const std::string& filepath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) { throw std::runtime_error(warn + err); }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t, std::hash<Vertex>> uniqueVertices{};
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};
                if (index.vertex_index >= 0) {
                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                }
                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }
                if (index.texcoord_index >= 0) {
                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }
                vertex.color = {1.0f, 1.0f, 1.0f};

                //todo removed the unique vertices due to bugs. no clue why tho
                if (uniqueVertices.count(vertex) == 0 && false) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex); 
                }
                vertices.push_back(vertex); 
                indices.push_back(index.vertex_index);
            }
        }
    }
}
