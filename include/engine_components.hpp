#pragma once
#include "glm/glm.hpp"

struct Transform {
        //glm::mat4 model{1.0f};
        glm::vec3 position{ 0.0f };
        glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 scale{ 1.0f };

        glm::mat4 matrix() const {
            glm::mat4 m{ 1.0f };
            m = glm::translate(m, position);
            m = glm::rotate(m, rotation.y, { 0,1,0 });
            m = glm::rotate(m, rotation.x, { 1,0,0 });
            m = glm::rotate(m, rotation.z, { 0,0,1 });
            m = glm::scale(m, scale);
            return m;
        }
    };
    struct Renderable{
        uint32_t meshID;
    };
    