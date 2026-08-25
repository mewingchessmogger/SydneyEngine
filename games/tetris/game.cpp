
#include "game.hpp"
#include "serde.hpp"
#include "sydney_physics.hpp"
void init(ECS::Registry& reg, EngineAPI& api){
	reg.createPool<Weapon>();
	
	api.loadModels({"dragon.glb","shibahu.glb","fps_character_animation_pack_ak-47.glb", "cube_gltf.glb", "sphere.glb"});
	

	TransformInfo t = {.position = glm::vec3(0.0, -5.0, 0.0), .rotation = {}, .scale = glm::vec3{ 3.0, 1.0, 4.0 } };
	int floor = reg.createEntity();
	
	reg.add(floor, t);
	Sydphys::Particle p{};
	p.inverseMass = 0.0;
	//reg.add(floor,p);
	api.attachModel("cube_gltf.glb", floor);
	
	int women = reg.createEntity();
	TransformInfo w = {.position = {0.0, 0.0, 0.0}, .rotation = {}, .scale = glm::vec3{ 1.0f }};
	reg.add(women, w);
	api.attachModel("shibahu.glb", women);
	api.setAnimation("Take 001", women);
	reg.add(women, Collider{.offset = {0.0, 1.0, 0.0}, .radius = 2.0f});

	int gun = reg.createEntity();
	TransformInfo g = {.position = {0.0,-2.0, -1.5},.rotation = {0.0, 180.0, 0.0}};

	reg.getPool<Weapon>().assign(gun,{gun,30});
	
	reg.add(gun, g);
	api.attachModel("fps_character_animation_pack_ak-47.glb", gun);
	api.setAnimation("RIG_UE5_Comando_AK_Idle", gun);
	reg.add(gun, Collider{.offset = {0.0, 1.0, 0.0}, .radius = 1.0f});

    api.setGameCamera(reg.createEntity());
	reg.emplace<TransformInfo>(api.getGameCamera());
	reg.emplace<Camera>(api.getGameCamera());
	
	reg.setParent(gun, api.getGameCamera());
	//reg.add(api.getGameCamera());
	

}
static std::array<int,20> CR_STATE children{};
static int CR_STATE count{};
void update(float aspect, float dt, Input::State &state, ECS::Registry& reg, EngineAPI& api)
{		

		Camera& camera = reg.getPool<Camera>().get(api.getGameCamera()); 
		TransformInfo& cameraPos = reg.getPool<TransformInfo>().get(api.getGameCamera()); 
		
		updateGameCamera(camera,cameraPos, state, 0.3, aspect, dt);
		updateWeaponSystem(state, api, reg);
		



		if(!reg.getPool<TransformInfo>().dense.size()){
			return; 
		}//TEMOPARRY BARRIER, SOMETIMES UPDATE CALLS WHEN


	

		TransformInfo& gun = reg.getPool<TransformInfo>().get(reg.getPool<ECS::Hierarchic>().dense[0]);
		Collider& womenCollider = reg.getPool<Collider>().data[0];
		Collider& gunCollider = reg.getPool<Collider>().data[1];
		if(state.keyPressed(Input::Key::MouseLeft)){
			
			Sydphys::Particle p = {.inverseMass = 1.0f, .vel = 3.0f * camera.dir, .acc = {0.0, -4.0,0.0}, .damping = 0.99f}; // c++20 forever
			TransformInfo t = {.position = camera.eye + 2.0f*camera.dir, .scale = {0.2,0.2,0.2}};
			int bull = reg.createEntity();
			reg.add(bull,t);
			reg.add(bull, p);
			api.attachModel("cube_gltf.glb", bull);
		}

		gun.setPos({0.0,-1.55, 0.02});
		gun.setRot({0.0, 180.0, 0.0});
		womenCollider.radius = 1.0f;
}

static bool CR_STATE alreadyInitialized = false;
static bool CR_STATE rejectStepExecution = false;

/*
NOTICE!!!!!!!!!


CR_LOAD WILL EXECTUE FIRSTLY FIRST STARTUP, BUT WHEN HOT RELOAD(ME REBUILD GAME) HAPPENS, CR_LOAD MIGHT NOT HAPPEN FIRST, DUE TO THIS AFTER HOT RELOAD,
MAKE SURE CR_STEP CASE IS NEVER INVOKED UNTIL NEXT CR_LOAD IS INVOKED AND  MANAGES TO DESERIALIZE/CREATE POOLS, ALSO DUE TO SPEED, IT
 CAN LOAD, FAIL, STEP, UNLOAD MULTIPLE TIMES BECAUES REBULILD STILL LOCKING DLL/PDB

 ALSO WEIRD QUIRK!!! INIT AND UPDATE CAN FOLLOW EACHOTHER INSIDE THE SAME FRAME!!! DONT THINK STEP WILL EXECUTE NEXT FRAME!! IT CAN EXEUCTE DIRECTLY AFTER INIT!!!;
  EG CR_MAIN CAN BE CALLED TWICE WITHIN THE SAME UPDATE FUNC
 
*/

CR_EXPORT int cr_main(cr_plugin *ctx, cr_op operation){
	PassedStructuresDLL* psd = static_cast<PassedStructuresDLL*>(ctx->userdata);	
	ECS::Registry& reg = *psd->reg;
	EngineAPI& api = *psd->api;
	Input::State& state = *psd->state; 
	float& aspect = *psd->aspect;
	float& dt = *psd->dt;
	RawMemory& raw = *psd->mem;
	GameState* mem = reinterpret_cast<GameState*>(raw.data);
	// mem->level = 100;
	// mem->MaxHP = 100.0f;
	// memcpy(mem->name, "Gunther", sizeof("Gunther"));
	// printf("my name is %s, my  level is %d and my hp is %f!!\n",mem->name ,mem->level, mem->MaxHP);

	//assert(sizeof(GameState) <= *psd->mem->bytes);


	switch (operation) 
		{
			case CR_LOAD:
			{
				printf("RELOADING DONE (OR FIRST LOAD)\n");
				if(ctx->failure != CR_BAD_IMAGE){
					if(!alreadyInitialized){
						init(reg, api);
						alreadyInitialized = true;
					}
				}
				else{
					printf("bad img still ..\n");
				}

			}
			break;

			case CR_STEP:{
				
				if(ctx->failure != CR_BAD_IMAGE && !rejectStepExecution){
					update(aspect, dt, state ,reg, api);
				}
			}
			break;

			case CR_UNLOAD:{
				printf("UNLOAD\n");
				
			}
			break;

			case CR_CLOSE:{
				reg.destroyPool<Weapon>();
			}
			break;
			
		}
	
    return 0;
}