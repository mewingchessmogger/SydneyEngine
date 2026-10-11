#pragma once 
#include "passed_structures_dll.hpp"
#include "engine_components.hpp"
#include "sydney_physics_components.hpp"
#include "cr/cr.h"




struct SaveData{
	float MaxHP{};
	int level{};
    enum Scene {NONE, MAIN_MENU, SCENE_1, SCENE_2, COCONUT} scene;  
};


struct RuntimeData{
	int Entities[16];
};

struct GameMemory{
    SaveData save;
    uint8_t pad0[128 * 1024 - sizeof(SaveData)];

    RuntimeData rnt;
    uint8_t pad1[896 * 1024 - sizeof(RuntimeData)];
};

inline void updateCameraOrientation(Camera& cam, const Input::State& state, float sens, float aspect){
    // 1. Mouse Look (Pitch & Yaw)
    cam.pitch += state.mouseDY * sens;
    cam.yaw   += state.mouseDX * sens;

    // Clamp pitch to prevent camera flipping
    if (cam.pitch >  89.0f) cam.pitch =  89.0f;
    if (cam.pitch < -89.0f) cam.pitch = -89.0f;

    // 2. Compute Direction Vectors
    glm::vec3 direction;
    direction.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    direction.y = sin(glm::radians(cam.pitch));
    direction.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));

    cam.dir   = glm::normalize(direction);
    cam.right = glm::normalize(glm::cross(cam.dir, cam.up)); // Precompute for movement
	
    // 3. Compute View & Projection Matrices
    cam.view = glm::lookAt(cam.eye, cam.eye + cam.dir, cam.up);
    cam.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

}

inline void updatePlayerMovement(SydX::Particle& particle, TransformInfo& transInfo, Camera& cam, Input::State& state, float moveSpeed){
    // Calculate planar forward (flattened on Y so moving forward doesn't fly up/down)
    glm::vec3 planarFront = glm::normalize(glm::vec3(cam.dir.x, 0.0f, cam.dir.z));

    glm::vec3 desiredVelocity(0.0f);
	
    if (state.keyHeld(Input::Key::C)) desiredVelocity += planarFront;
    if (state.keyHeld(Input::Key::X)) desiredVelocity -= planarFront;
    if (state.keyHeld(Input::Key::Z)) desiredVelocity -= cam.right;
    if (state.keyHeld(Input::Key::V)) desiredVelocity += cam.right;
	
    // Normalize diagonal movement so holding two keys isn't 1.41x faster
    if (glm::length(desiredVelocity) > 0.0001f) {
        desiredVelocity = glm::normalize(desiredVelocity) * moveSpeed;
    }

	particle.vel.x = desiredVelocity.x;
    // Apply directly to velocity component
    particle.vel.z =  desiredVelocity.z;

	cam.eye = transInfo.position;
    
}
inline void updateGameCamera(Camera& cam, TransformInfo& camTrans, Input::State& state, float sens, float aspect, float dt)
{
        // float aspect = plt.aspectRatio;
		// float dt = plt.deltaTime;
		// Input::State &state = plt.inputState;

        float& pitch = cam.pitch;
        float& yaw = cam.yaw;
        
		pitch += state.mouseDY * 0.3;
		yaw += state.mouseDX * 0.3;

		if(pitch > 89.0f)
			pitch =  89.0f;
		if(pitch < -89.0f)
			pitch = -89.0f;

		

		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);

		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		glm::vec3 cameraFront = glm::normalize(direction);
		
		glm::vec3& eye = cam.eye;
		glm::vec3& cameraUp = cam.up;

	
		float sensitivity = 1.0f;

		float cameraYAxis{};
		

		if (state.keyHeld(Input::Key::C)){
			eye += glm::vec3(cameraFront.x, cameraYAxis, cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::X)){
			eye -= glm::vec3(cameraFront.x, cameraYAxis, cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::Z)){
			eye -= glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;

		}
		if (state.keyHeld(Input::Key::V)){
			eye += glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;
		}
		
		glm::mat4 camPos = inverse(glm::lookAt(eye, cameraFront+eye, cameraUp));

		camTrans.position.x = camPos[3].x;
		camTrans.position.z = camPos[3].z;
		eye.y = camTrans.position.y;
		
		camTrans.dirty = true;

		camTrans.setRot(glm::vec3(glm::degrees(glm::eulerAngles(glm::quat_cast(camPos)))));
		
		cam.view = glm::lookAt(eye, cameraFront+eye, cameraUp);
		cam.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		cam.dir = cameraFront;
}


struct Weapon{
	int bullets = 30;
	REFLECT_ADDRESS_1(bullets);
	COMP_NAME(Weapon);
};

void init(ECS::Registry& reg, EngineAPI& api) ;
void update(float aspect, float dt, Input::State &state, ECS::Registry &reg, EngineAPI& api) ;

void updateWeaponSystem(Input::State & state, EngineAPI & api, ECS::Registry &reg, RuntimeData& rnt);
