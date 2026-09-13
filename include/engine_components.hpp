#pragma once
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include "reflections.hpp"



struct TransformInfo{
    glm::vec3 position{ 0.0f };
    glm::quat quatRot{};
    glm::vec3 scale{ 1.0f };
    uint32_t dirty = true;
    
    void setPos(const glm::vec3& newPosition) {dirty = true; position = newPosition;}
    void setRot(const glm::vec3& newRotation) {dirty = true; quatRot = glm::quat(glm::radians(newRotation));}
    void setScale(const glm::vec3& newScale)  {dirty = true; scale = newScale;}
    
    void addPos(const glm::vec3& newPosition) {dirty = true; position += newPosition;}
    

    void addRot(const glm::vec3& newRotation) {
        dirty = true;
        glm::vec3 rad = glm::radians(newRotation);
        float angle = glm::length(rad);
        if (angle < 0.0001f){ // when dt is supah small, this will return, 
            return;
        }
        glm::vec3 axis = glm::normalize(rad); 
        glm::quat delta = glm::angleAxis(angle, axis);
        quatRot = glm::normalize(quatRot * delta);
    }
    void addRot(const glm::vec3& newRotation, float dt) {
        addRot(newRotation * dt);
    }
    void addScale(const glm::vec3& newScale)  {
        dirty = true; scale += newScale;
    }


    glm::mat4 getLocalMatrix() const {
        glm::mat4 m{ 1.0f };
        m = glm::translate(m, position);
        m = m * glm::mat4_cast(quatRot);
        m = glm::scale(m, scale);
        return m;
    }

    //REFLECT_4(position, rotation, scale, dirty); 
    COMP_NAME(TransformInfo);
};

struct RawTransform {
    glm::mat4 matrix{}; //split up cuz i want this to be the what packets source transforms from, other alternative is having this inside TransformInfo, 
    
    inline glm::mat3 getLinear() const { 
        return glm::mat3(matrix); 
    }
    
    inline glm::vec3 getPosition() const { 
        return glm::vec3(matrix[3]); 
    }
    
    inline glm::vec3 getScale() const {
        glm::mat3 L = getLinear();
        return glm::vec3(glm::length(L[0]), glm::length(L[1]), glm::length(L[2]));
    }
    
    inline glm::mat3 getRotationMatrix() const {
        glm::mat3 L = getLinear();
        glm::vec3 s = getScale();
        return glm::mat3(L[0] / s.x, L[1] / s.y, L[2] / s.z);
    }
    
    COMP_NAME(RawTransform);
    
};

struct Collider{
   
    //BROADCOLLIDER
    glm::vec3 offset{};
    float broadRadius = 1.0f;
    
    //NARROW COLLIDER
    enum Shape {NONE, SPHERE, OBB, AABB, COCONUT} narrowShape = NONE;  
    float narrowRadius = 0.2f;
    glm::vec3 narrowExtents = glm::vec3(1.0f);
    
    //narrowCollider


    COMP_NAME(Collider);
};


struct Camera {
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


