#pragma once 
#include "glm/glm.hpp"

;
namespace physics {
    using real = float;
    using vec3 = glm::vec3;
        
    class Particle {
    public:
        // in future may change to double to enhance precision of calcs

        void integrate(float dt, bool accurateDamping);


        real inverseMass{};
        vec3 pos{};
        vec3 vel{};
        vec3 acc{};
        vec3 forceAccumulator{};
        real damping{};
    };

    class ParticleForceGenerator{
        public: 
        virtual void updateForce(Particle* particle, real dt) = 0;
    };

    class ParticleForceRegistry{

        protected: 

        struct ParticleForceRegistration{
            Particle* particle ;
            ParticleForceGenerator* fg;
        };

        std::vector<ParticleForceGenerator> registrations{};
        void add(Particle* particle, ParticleForceGenerator* fg);

        void remove(Particle* particle, ParticleForceGenerator* fg);
        void clear();
        void updateForces(real dt);


    };
};