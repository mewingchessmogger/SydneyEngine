
#include "game.hpp"
#include "serde.hpp"

void init(ECS::Registry& reg, EngineAPI& api){
	reg.createPool<Weapon>();
	
	api.loadModels({"models/dragon.glb","models/shibahu.glb","models/fps_character_animation_pack_ak-47.glb"});
	

	Transform t = {.position = glm::vec3(3.0, -3.0, 0.0), .rotation = {}, .scale = glm::vec3{ 4.0, 1.0, 4.0 } };
	//int floor = reg.createEntity();
	// reg.add(floor, t);
	// api.attachModel("models/cube_gltf.glb", floor);
	
	
	int women = reg.createEntity();
	Transform w = {.position = {2.0, -1.5, 0.0}, .rotation = {}, .scale = glm::vec3{ 2.0f }};
	reg.add(women, w);
	api.attachModel("models/shibahu.glb", women);
	api.setAnimation("Take 001", women);
		
	int gun = reg.createEntity();
	Transform g = {.position = {0.0,-2.0, -1.5},.rotation = {0.0, 180.0, 0.0}};

	reg.getPool<Weapon>().assign(gun,{gun,30});

	reg.add(gun, g);
	api.attachModel("models/fps_character_animation_pack_ak-47.glb", gun);
	api.setAnimation("RIG_UE5_Comando_AK_Idle", gun);
	

    api.setGameCamera(reg.createEntity());
	reg.emplace<Transform>(api.getGameCamera());
	reg.emplace<Camera>(api.getGameCamera());
	Node n = {.child = gun};

	reg.add(api.getGameCamera(),n);
	

}

void update(float aspect, float dt, Input::State &state, ECS::Registry& reg, EngineAPI& api)
{		

		Camera& camera = reg.getPool<Camera>().get(api.getGameCamera()); 
		Transform& cameraPos = reg.getPool<Transform>().get(api.getGameCamera()); 
		updateGameCamera(camera,cameraPos, state, 0.3, aspect, dt);
		updateWeaponSystem(state, api, reg);
		Transform& gun = reg.getPool<Transform>().get(reg.getPool<Node>().data[0].child);
		gun.position =  {0.0,-1.55, 0.02};
		gun.rotation = {0.0, 180.0, 0.0};
		

		// if (state.keyPressed(Input::Key::Jump) || state.keyPressed(Input::Key::LeftClick)){
		// 	int dragon = reg.createEntity();

		// 	Transform T{};
		// 	glm::vec3 floorForward = glm::normalize(glm::vec3(camera.dir.x, 0.0f, camera.dir.z));
		// 	glm::vec3 eyeFloor = glm::vec3(camera.eye.x,0.0,camera.eye.z);
		// 	T.position = eyeFloor + floorForward;
		// 	T.scale = {20.0f,2.0f,2.0f};
		// 	T.rotation = {0.0,0.0,0.0};
		// 	Particle p{};
		// 	p.pos = eyeFloor;
		// 	p.vel = floorForward * 2.0f;
		// 	p.acc = {0.0,-4.0,0.0};
		// 	p.damping = 0.5;
		// 	p.inverseMass = 15;
			
		// 	api.attachModel("models/dragon.glb", dragon);
		// 	reg.add(dragon,T,p);
		


		// }

		// auto& PPool = reg.getPool<physics::Particle>();

        // for (int i{}; i < PPool.count; i++){
        //     ECS::Entity e = PPool.dense[i];
        //     Particle& p = PPool.data[i];
		// 	glm::vec3 vel =p.vel;

        // }

	
}


static bool CR_STATE alreadyInitialized = false;
static bool CR_STATE rejectStepExecution = false;
/*
NOTICE!!!!!!!!!


CR_LOAD WILL EXECTUE FIRSTLY FIRST STARTUP, BUT WHEN HOT RELOAD(ME REBUILD GAME) HAPPENS, CR_LOAD MIGHT NOT HAPPEN FIRST, DUE TO THIS AFTER HOT RELOAD,
MAKE SURE CR_STEP CASE IS NEVER INVOKED UNTIL NEXT CR_LOAD IS INVOKED AND  MANAGES TO DESERIALIZE/CREATE POOLS, ALSO DUE TO SPEED, IT
 CAN LOAD, FAIL, STEP, UNLOAD MULTIPLE TIMES BECAUES REBULILD STILL LOCKING DLL/PDB

 ALSO WEIRD QUIRK!!! INIT AND UPDATE CAN FOLLOW EACHOTHER INSIDE THE SAME FRAME!!! DONT THINK STEP WILL EXECUTE NEXT FRAME!! IT CAN EXEUCTE DIRECTLY AFTER INIT!!!;
 
*/
CR_EXPORT int cr_main(cr_plugin *ctx, cr_op operation){
	PassedStructuresDLL* psd = static_cast<PassedStructuresDLL*>(ctx->userdata);	
	
	ECS::Registry& reg = *psd->reg;
	EngineAPI& api = *psd->api;
	Input::State& state = *psd->state; 
	float& aspect = *psd->aspect;
	float& dt = *psd->dt;
	
	
	switch (operation) 
		{
			case CR_LOAD:
			{
				printf("RELOADING DONE (OR FIRST LOAD)\n");
				if(ctx->failure != CR_BAD_IMAGE){
					if(!alreadyInitialized){
						init(reg, api);
						alreadyInitialized = true;
						
					}else{
						rejectStepExecution = false;
						 printf("goo1");
						reg.createPool<Weapon>();
						std::ifstream is("games/tetris/temp.txt");
						auto packets = std::move(Serde::deserializeFile(is));    				
						printf("goo12\n");
						is.close();
						
						for(auto& pkt: packets){
							reg.deserializeComponent(pkt.id,pkt.sName,std::move(pkt.vars));
						
						}
						
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
				if(!rejectStepExecution){
					auto& wPool = reg.getPool<Weapon>();	
					std::ofstream os("games/tetris/temp.txt"); // std::ios_base::app
					
					Serde::serializePool<Weapon>(os, reg);		
					os.close();

					reg.destroyPool<Weapon>();
					rejectStepExecution = true;
				}
				
					

			}
			break;

			case CR_CLOSE:{
				reg.destroyPool<Weapon>();
			}
			break;
			
		}
	
    return 0;
}