#pragma once
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"
#include "SwapChain.h"
#include "Texture.h"


namespace svk {

    struct TransfromComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};
        
        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float lightIntensity = 0.5f;
    };
    
    struct GameObjectBufferData {
        glm::mat4 modelMatrix{1.0f};
        glm::mat4 normalMatrix{1.0f};
    };

    class GameObjectManager; 
    
    class GameObj {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObj>;
        
        GameObj(GameObj &&) = default;
        GameObj(const GameObj &) = delete;
        GameObj &operator=(const GameObj &) = delete;
        GameObj &operator=(GameObj &&) = delete;

        id_t getId(){ return id; }

        VkDescriptorBufferInfo getBufferInfo(int frameIndex);
        
        std::shared_ptr<Model> model{};
        std::shared_ptr<Texture> diffuseMap = nullptr;
        
        std::shared_ptr<Texture> specularMap = nullptr;
        
        glm::vec3 color{};
        TransfromComponent transform{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;
        
    private:
        GameObj(id_t objId, const GameObjectManager &manager);

        id_t id;
        const GameObjectManager &gameObjectManger;
        friend class GameObjectManager;
    };

    class GameObjectManager {
    public:
        static constexpr int MAX_GAME_OBJECTS = 1000;

        GameObjectManager(Device &device);
        GameObjectManager(const GameObjectManager &) = delete;
        GameObjectManager &operator=(const GameObjectManager &) = delete;
        GameObjectManager(GameObjectManager &&) = delete;
        GameObjectManager &operator=(GameObjectManager &&) = delete;

        GameObj &createGameObject() {
            assert(currentId < MAX_GAME_OBJECTS && "Max game object count exceeded!");
            usedIds.push_back(currentId);
            auto gameObject = GameObj{currentId++, *this};
            
            auto gameObjectId = gameObject.getId();
            gameObject.diffuseMap = textureDefault;
            gameObjects.emplace(gameObjectId, std::move(gameObject));
            return gameObjects.at(gameObjectId);
        }

        GameObj &makePointLight(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

        VkDescriptorBufferInfo getBufferInfoForGameObject(int frameIndex, GameObj::id_t gameObjectId) const {
            return uboBuffers[frameIndex]->descriptorInfoForIndex(gameObjectId);
        }

        void updateBuffer(int frameIndex);

        GameObj::Map gameObjects{};
        std::vector<std::unique_ptr<Buffer>> uboBuffers{SwapChain::MAX_FRAMES_IN_FLIGHT};
        std::vector<GameObj::id_t> usedIds;

    private:
        GameObj::id_t currentId = 0;
        std::shared_ptr<Texture> textureDefault;
    };
}
