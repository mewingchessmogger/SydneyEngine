#pragma once 
#include "passed_structures_dll.hpp"
#include "fysik_motor.hpp"
#include "engine_components.hpp"

// #include "glm/gtc/quaternion.hpp"
// #include "glm/gtx/quaternion.hpp"
// #include <glm/gtx/projection.hpp>

// #include "game_memory.hpp"
// #include "reflections.hpp"
#include "camera.hpp"
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
 
// #define  GLM_FORCE_RADIANS
// #define  GLM_ENABLE_EXPERIMENTAL
//#include "script_component.hpp"
#include "cr/cr.h"


struct Weapon{
	int id{};
	int bullets = 30;

};
void init(ECS::Registry& reg, EngineAPI& api) ;
void update(float aspect, float dt, Input::State &state, ECS::Registry &reg, EngineAPI& api) ;

void updateWeaponSystem(Input::State & state, EngineAPI & api, ECS::Registry &reg);
