#pragma once
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"


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
        float lightIntensity = 10.0f;
    };
    
    class GameObj {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObj>;
        
        static GameObj createGameObj() {
            static id_t currentId = 0;
            return GameObj{currentId++};
        }

        static GameObj makePointLight(float intensity = 1.0f, float radius = 0.1f, glm::vec3 color = glm::vec3{1.0f});

        GameObj(const GameObj &) = delete;
        GameObj &operator=(const GameObj &) = delete;
        GameObj(GameObj &&) = default;
        GameObj &operator=(GameObj &&) = default;

        id_t getId(){ return id; }

        std::shared_ptr<Model> model{};
        //todo heeres the image
        std::shared_ptr<VkImage> textureImage{};
        
        glm::vec3 color{};
        TransfromComponent transform{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;
        
    private:
        GameObj(id_t objId) : id{objId} {}

        id_t id;
    };
}
