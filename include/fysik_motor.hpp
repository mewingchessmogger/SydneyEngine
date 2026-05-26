#pragma once 
#include "glm/glm.hpp"

class Particle {
public:
    using vec3 = glm::vec3;
    using real = float; // in future may change to double to enhance precision of calcs

    void integrate(float dt, bool accurateDamping);


    real inverseMass{};
    vec3 pos{};
    vec3 vel{};
    vec3 acc{};
    vec3 forceAccumulator{};
    real damping{};
};