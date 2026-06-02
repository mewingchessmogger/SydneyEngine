#include "fysik_motor.hpp"

/**
     * A particle is the simplest object that can be simulated in the
     * physics system.
     *
     * It has position data (no orientation data), along with
     * velocity. It can be integrated forward through time, and have
     * linear forces, and impulses applied to it. The particle manages
     * its state and allows access through a set of methods.
     */
using Particle = physics::Particle;
void Particle::integrate(float dt, bool accurateDamping){ // powf is slow so if calculating many objects it may be too slow
        
        if (inverseMass <= 0.0f) return;

        assert(dt >= 0);
        // p is position 
        // p' means derivative of p, p'' means second derivative and so on...
        
        pos += vel * dt;  // p = p + p'*t + [(1/2) * p''*t^2] <- acc part usually neglibigle in pos update so we ignore 
        vec3 resultingAcceleration = acc;
        resultingAcceleration += forceAccumulator * inverseMass;

        vel += resultingAcceleration * dt; // p' = p' * d + p'' * t
        
        
        // CODE DOES THIS BUT THIS DOESNT FOLLOW MATH correctly! CHECK PAGE 53 IN IAN MILLINGTON
        if (accurateDamping){
            vel *= powf(damping,dt);    
        }else{
            vel *= damping;
        }
        
        if ((vel.x *vel.x + vel.y* vel.y +vel.z* vel.z) < 0.0001f) {
                vel = vec3(0.0f);
        }

        forceAccumulator = {};

        
}
