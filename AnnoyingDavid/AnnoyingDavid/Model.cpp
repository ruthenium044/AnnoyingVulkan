﻿#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION   
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "Renderer.h"

namespace std {
    template <>
    struct hash<svk::Model::Vertex> {
        size_t operator()(const svk::Model::Vertex& vertex) const {
            size_t seed = 0;
            svk::hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.texCoord);
            return seed;
        }
    };
}

namespace svk {
    std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescription(1);
        bindingDescription[0].binding = 0;
        bindingDescription[0].stride = sizeof(Vertex);
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        return attributeDescriptions;
    }

    Model::Model(Device& dev, const Builder& builder)
        : device(dev) {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    Model::~Model() {}

    std::unique_ptr<Model> Model::createModelFromFile(Device& device, const std::string& filepath) {
        Builder builder;
        builder.loadModel(filepath);
        return std::make_unique<Model>(device, builder);
    }

    void Model::draw(VkCommandBuffer commandBuffer) {
        if (hasIndexBuffer) { vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0); }
        else { vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0); }
    }

    void Model::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[]{vertexBuffer->getBuffer()};
        VkDeviceSize offsets[]{0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if (hasIndexBuffer) { vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32); }
    }

    void Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);

        Buffer stagingBuffer{
            device, vertexSize, vertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer(vertices.data());

        vertexBuffer = std::make_unique<Buffer>(device, vertexSize, vertexCount,
                                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
    }

    void Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        if (!hasIndexBuffer) { return; }
        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
        uint32_t indexSize = sizeof(indices[0]);

        Buffer stagingBuffer{
            device, indexSize, indexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };
        stagingBuffer.map();
        stagingBuffer.writeToBuffer(indices.data());

        indexBuffer = std::make_unique<Buffer>(device, indexSize, indexCount,
                                               VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
    }
    
    void Model::Builder::loadModel(const std::string& filepath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
            throw std::runtime_error(warn + err);
        }

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
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }
                vertex.color = {1.0f, 1.0f, 1.0f};

                //todo removed the unique vertices due to bugs. no clue why tho
                //if (uniqueVertices.count(vertex) == 0 && false) {
                //    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                //    vertices.push_back(vertex); 
                //}
                vertices.push_back(vertex);
                //indices.push_back(index.vertex_index);
            }
        }
    }
}
