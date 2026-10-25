#pragma once 
#include "reflections.hpp"
#include "glm/matrix.hpp"      
#include <glm/gtx/norm.hpp>
namespace SydX{
    using real = float;
    using vec3 = glm::vec3;

  struct Particle{
        real inverseMass{};
        vec3 vel{};
        vec3 acc{};
        real friction{};
        float bounciness = 0.5; // 1.0f : completely elastic, 0.0f : Everything becomes heat
        REFLECT_ADDRESS_5(inverseMass, vel, acc, friction, bounciness);
        COMP_NAME(Particle);
    };
    
    struct RigidBody {
        //glm::mat3 inertiaTensor{};
        float inverseMass{};
        glm::quat rotation{};
        glm::vec3 position{};
        glm::vec3 prevPosition{};
        glm::vec3 velocity{};
        //glm::vec3 angularVelocity{};
        //glm::vec3 inverseInertia{};
        //float friction{};
        //float bounciness{};
        COMP_NAME(RigidBody)
    };
};
