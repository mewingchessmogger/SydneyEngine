#include "fysik_motor.hpp"

void Particle::setPosition(const vec3& position) {
    pos = position;
}

void Particle::setVelocity(const vec3& velocity) {
    vel = velocity;
}

void Particle::setAcceleration(const vec3& acceleration) {
    acc = acceleration;
}

void Particle::setForceAccumulator(const vec3& force) {
    forceAccumulator = force;
}

void Particle::setDamping(real dampingValue) {
    damping = dampingValue;
}

void Particle::setInverseMass(real inverseMassValue) {
    inverseMass = inverseMassValue;
}