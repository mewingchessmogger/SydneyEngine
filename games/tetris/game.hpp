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
	
	std::array<void(*)(int, int, ECS::Registry*),10> callbacks{};
};

struct GameMemory{
    SaveData save;
    uint8_t pad0[128 * 1024 - sizeof(SaveData)];

    RuntimeData rnt;
    uint8_t pad1[896 * 1024 - sizeof(RuntimeData)];
};



inline void updateGameCamera(Camera& cam, TransformInfo& camTrans, Input::State& state, float sens, float aspect, float dt)
{
        
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
		
		//glm::vec3& eye = cam.eye;
		glm::vec3& cameraUp = cam.up;

	
		float sensitivity = 1.0f;

		float cameraYAxis{};
		

		if (state.keyHeld(Input::Key::C)){
			camTrans.position += glm::vec3(cameraFront.x, cameraYAxis, cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::X)){
			camTrans.position -= glm::vec3(cameraFront.x, cameraYAxis, cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::Z)){
			camTrans.position -= glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;

		}
		if (state.keyHeld(Input::Key::V)){
			camTrans.position += glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;
		}
		
		camTrans.setRot(glm::vec3(pitch, yaw, 0.0f));
		camTrans.dirty = true;
		//camTrans.position.x = camPos[3].x;
		//camTrans.position.z = camPos[3].z;
		//camTrans.setRot(cameraFront);
		
		
		cam.view = glm::lookAt(camTrans.position, cameraFront + camTrans.position, cameraUp);
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
