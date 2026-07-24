#pragma once 
#include "passed_structures_dll.hpp"
#include "fysik_motor.hpp"
#include "engine_components.hpp"

#include "camera.hpp"

#include "cr/cr.h"


struct Weapon{
	int id{};
	int bullets = 30;
	REFLECT_2(id, bullets);
	REFLECT_ADDRESS_2(id,bullets);
};
void init(ECS::Registry& reg, EngineAPI& api) ;
void update(float aspect, float dt, Input::State &state, ECS::Registry &reg, EngineAPI& api) ;

void updateWeaponSystem(Input::State & state, EngineAPI & api, ECS::Registry &reg);
