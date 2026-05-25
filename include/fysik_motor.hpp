#include "glm/glm.hpp"

class Particle {
public:
    using vec3 = glm::vec3;
    using real = float; // in future may change to double to enhance precision of calcs

    void integrate(float dt, bool accurateDamping);

    // --- SETTERS ---
    
    // Describes position, velocity and acceleration in WORLD SPACE
    void setPosition(const vec3& position);
    void setVelocity(const vec3& velocity);
    void setAcceleration(const vec3& acceleration);
    
    void setForceAccumulator(const vec3& force);

    /**
    * Holds the amount of damping applied to linear
    * motion. Damping is required to remove energy added
    * through numerical instability in the integrator.
    */
    void setDamping(real dampingValue);
    
    /* 1/m more useful to store to get infinite mass easily by setting inversemass var to 0! like walls and floors not being able to move! */
    void setInverseMass(real inverseMassValue);

protected:
    real inverseMass{};
    vec3 pos{};
    vec3 vel{};
    vec3 acc{};
    vec3 forceAccumulator{};
    real damping{};
};