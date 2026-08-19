#pragma once
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include "reflections.hpp"



struct TransformInfo{
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f, 0.0f, 0.0f }; // IN DEGREES example : {90, 45, 0} NOT from 0 to 1 or euler 0 to 3.14
    glm::vec3 scale{ 1.0f };
    uint32_t dirty = true;
    
    void setPos(const glm::vec3& newPosition) {dirty = true; position = newPosition;}
    void setRot(const glm::vec3& newRotation) {dirty = true; rotation = newRotation;}
    void setScale(const glm::vec3& newScale)  {dirty = true; scale = newScale;}
    
    void addPos(const glm::vec3& newPosition) {dirty = true; position += newPosition;}
    void addRot(const glm::vec3& newRotation) {dirty = true; rotation += newRotation;}
    void addScale(const glm::vec3& newScale)  {dirty = true; scale += newScale;}


    glm::mat4 getLocalMatrix() const {
        glm::mat4 m{ 1.0f };
        m = glm::translate(m, position);
        glm::quat qRotation = glm::quat(glm::radians(glm::vec3(rotation.x, rotation.y, rotation.z)));
        m = m * glm::mat4_cast(qRotation);
        m = glm::scale(m, scale);
        return m;
    }

    REFLECT_4(position, rotation, scale, dirty); 
    COMP_NAME(TransformInfo);
};

struct RawTransform {
    glm::mat4 matrix{}; //split up cuz i want this to be the what packets source transforms from, other alternative is having this inside TransformInfo, 
    
    glm::mat3 linear() const { 
        return glm::mat3(matrix); 
    }
    
    glm::vec3 position() const { 
        return glm::vec3(matrix[3]); 
    }
    
    glm::vec3 scale() const {
        glm::mat3 L = linear();
        return glm::vec3(glm::length(L[0]), glm::length(L[1]), glm::length(L[2]));
    }
    
    glm::mat3 rotationMatrix() const {
        glm::mat3 L = linear();
        glm::vec3 s = scale();
        return glm::mat3(L[0] / s.x, L[1] / s.y, L[2] / s.z);
    }

    COMP_NAME(RawTransform);
    
};

struct Collider{
    glm::vec3 halfExtents{0.5f};  // Box
    float     radius{0.5f};        // Sphere, Capsule
    float     halfHeight{0.5f};    // Capsule
    COMP_NAME(Collider);
};


struct Camera {
    glm::mat4 model{};
    glm::mat4 view{};
    glm::mat4 proj{};
    glm::vec3 eye = glm::vec3(0.0f, 1.0f, 2.0f);
    glm::vec3 dir = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float pitch{};
    float yaw{ };
    COMP_NAME(Camera);
};
struct Renderable{
    uint32_t id{};
    REFLECT_1(id);

    COMP_NAME(Renderable);
};

struct Parent{
    uint32_t parentID{};
    uint32_t level{};
    REFLECT_2(parentID, level);
    COMP_NAME(Parent);
};

struct Animated{
    int animationIndex{};
    float time{}; // in seconds
    float speed = 1.0f;
    float duration{}; // in seconds

    uint32_t offset{}; // after how many mat4s does the next animation start?  [0] = 0, [1] = 128 [2] = 2500
	uint16_t totalFrames{};
    int layer = -1;
    float crossfade = 0.0;
    bool isLocked = false;


    int getFrame() const{
        float progress = time / duration;
        int frameIndex = static_cast<int>(progress * totalFrames) % totalFrames;

        //printf("progress :%f , totalFrames: %d frameIndex: %d \n", progress, totalFrames,frameIndex );

        return frameIndex;
    }
    COMP_NAME(Animated);
};


