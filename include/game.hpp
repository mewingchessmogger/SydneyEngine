#pragma once 
#include "scene.hpp"
#include "input_format.hpp"
#include "ecs_registry.hpp"
#include "hash_model.hpp"
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
 
// #define  GLM_FORCE_RADIANS
// #define  GLM_ENABLE_EXPERIMENTAL

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <glm/gtx/projection.hpp>

void initGame(Scene &scn, ECS::Registry &reg);
void updateGame(Scene &scn, float aspect, float dt, Input::State &state, ModelStorage &storage, ECS::Registry &reg);

