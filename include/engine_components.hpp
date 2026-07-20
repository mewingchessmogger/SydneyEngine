#pragma once
//#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "input_format.hpp"
#include "memory"
#include "game_memory.hpp"
#include "string"

#include "reflections.hpp"

struct Transform {
    //glm::mat4 model{1.0f};
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f, 0.0f, 0.0f }; // IN DEGREES example : {90, 45, 0} NOT from 0 to 1 or euler 0 to 3.14
    glm::vec3 scale{ 1.0f };

    glm::mat4 matrix() const {
        glm::mat4 m{ 1.0f };
        m = glm::translate(m, position);
        glm::quat qRotation = glm::quat(glm::radians(glm::vec3(rotation.x, rotation.y, rotation.z)));
        m = m * glm::mat4_cast(qRotation);
        m = glm::scale(m, scale);
        return m;
    }

    REFLECT_3(position, rotation, scale);


};
struct Camera {
    glm::mat4 model{};
    glm::mat4 view{};
    glm::mat4 proj{};
    glm::vec3 eye = glm::vec3(0.0f, 0.0f, 2.0f);
    glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float pitch{};
    float yaw{ };
};
struct Renderable{
    uint32_t id{};
    REFLECT_1(id);
};

struct Parent{
    uint32_t parentID{};
    uint32_t level{};
    REFLECT_2(parentID, level);
};

struct Name{
 std::string name{};
};
    
struct Animated{
    int animationIndex{};
    float time{}; // in seconds
    float speed = 1.0f;
    float duration{}; // in seconds
    
    uint32_t offset{}; // after how many mat4s does the next animation start?  [0] = 0, [1] = 128 [2] = 2500
	uint16_t totalFrames{};

    int getFrame(){
        float progress = time / duration;
        int frameIndex = static_cast<int>(progress * totalFrames) % totalFrames;

        //printf("progress :%f , totalFrames: %d frameIndex: %d \n", progress, totalFrames,frameIndex );

        return frameIndex;
    }

};

