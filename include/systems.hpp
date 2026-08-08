#pragma once
#include "engine_api.hpp"
#include "engine_components.hpp"
#include "asset_registry.hpp"
#include "ecs_registry.hpp"
#include "asset_loader_interface.hpp"
#include "render_packet_def.hpp"
namespace Sys{
    
    void processAPI(ECS::Registry& reg, AssetRegistry& ast, EngineAPI& api, IAssetLoader& loader);
    void buildRenderPackets(ECS::Registry& reg, AssetRegistry& ast, std::vector<RenderPkt>& packets);
    void updateAnimations(ECS::Registry& reg, float dt);
    void updatePhysics(ECS::Registry& reg, float dt);
    void finalizeTransforms(ECS::Registry& reg);
    
};