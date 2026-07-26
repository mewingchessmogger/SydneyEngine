#pragma once 
#include "passed_structures_dll.hpp"
#include "engine_components.hpp"
#include "cr/cr.h"



inline void updateGameCamera(Camera& cam, Transform& camTrans, Input::State& state, float sens, float aspect, float dt)
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
		

		if (state.keyHeld(Input::Key::W)){
			eye += glm::vec3(cameraFront.x, cameraYAxis, cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::S)){
			eye -= glm::vec3(cameraFront.x, cameraYAxis, cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::A)){
			eye -= glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;

		}
		if (state.keyHeld(Input::Key::D)){
			eye += glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;
		}
		
		glm::mat4 camPos = inverse(glm::lookAt(eye, cameraFront+eye, cameraUp));

		camTrans.worldPosition = glm::vec3{camPos[3]};
		camTrans.worldRotation= glm::degrees(glm::eulerAngles(glm::quat_cast(camPos)));
		
		cam.view = glm::lookAt(eye, cameraFront+eye, cameraUp);
		cam.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		cam.dir = cameraFront;
}


struct Weapon{
	int id{};
	int bullets = 30;
	REFLECT_2(id, bullets);
	REFLECT_ADDRESS_2(id,bullets);
};
void init(ECS::Registry& reg, EngineAPI& api) ;
void update(float aspect, float dt, Input::State &state, ECS::Registry &reg, EngineAPI& api) ;

void updateWeaponSystem(Input::State & state, EngineAPI & api, ECS::Registry &reg);
