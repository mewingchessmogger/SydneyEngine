#include "fysik_motor.hpp"


void Particle::integrate(float dt, bool accurateDamping){ // powf is slow so if calculating many objects it may be too slow
        assert(dt >= 0);
        // p is position 
        // p' means derivative of p, p'' means second derivative and so on...
        
        pos += vel * dt;  // p = p + p'*t + [(1/2) * p''*t^2] <- acc part usually neglibigle in pos update so we ignore 
        vec3 resultingAcceleration = acc;
        resultingAcceleration += forceAccumulator * inverseMass;

        vel += resultingAcceleration * dt; // p' = p' * d + p'' * t
        
        if (accurateDamping){
                vel *= powf(damping,dt);
                return;
        }
        
        vel *= damping; // CODE DOES THIS BUT THIS DOESNT FOLLOW MATH correctly! CHECK PAGE 53 IN IAN MILLINGTON
}
