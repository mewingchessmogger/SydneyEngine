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
        real damping{};
        float bounciness = 0.5; // 1.0f : completely elastic, 0.0f : Everything becomes heat
        REFLECT_ADDRESS_5(inverseMass, vel, acc, damping, bounciness);
        COMP_NAME(Particle);
    };

};
