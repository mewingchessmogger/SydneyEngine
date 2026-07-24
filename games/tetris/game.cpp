
#include "game.hpp"
#include "serde.hpp"

using Particle = physics::Particle;
struct Bullet{
	int speed{};
	REFLECT_1(speed);
};


void init(ECS::Registry& reg, EngineAPI& api){
	reg.createPool<Weapon>();
	
	api.loadModels({"models/cube_gltf.glb","models/dragon.glb","models/shibahu.glb","models/fps_character_animation_pack_ak-47.glb"});
	
	//int floor = reg.createEntity();

	Transform t = {.position = glm::vec3(3.0, -3.0, 0.0), .rotation = {}, .scale = glm::vec3{ 4.0, 1.0, 4.0 } };
	// reg.add(floor, t);
	// api.attachModel("models/cube_gltf.glb", floor);
	
	
	int women = reg.createEntity();
	Transform w = {.position = {2.0, -1.5, 0.0}, .rotation = {}, .scale = glm::vec3{ 2.0f }};
	reg.add(women, w);
	api.attachModel("models/shibahu.glb", women);
	api.setAnimation("Take 001", women);
		
	int gun = reg.createEntity();
	Transform g = {.position = {1.0, -1.5,0.0}};
	reg.add<Weapon>(gun, {gun, 30});

	reg.add(gun, g);
	api.attachModel("models/fps_character_animation_pack_ak-47.glb", gun);
	api.setAnimation("RIG_UE5_Comando_AK_Reload", gun);
	

    api.setGameCamera(reg.createEntity());
	reg.emplace<Camera>(api.getGameCamera());

}

void update(float aspect, float dt, Input::State &state, ECS::Registry& reg, EngineAPI& api)
{		

		Camera& camera = reg.getPool<Camera>().get(api.getGameCamera()); 
		updateGameCamera(camera, state, 0.3, aspect, dt);
		updateWeaponSystem(state, api, reg);

		

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

		auto& PPool = reg.getPool<physics::Particle>();

        for (int i{}; i < PPool.count; i++){
            ECS::Entity e = PPool.dense[i];
            Particle& p = PPool.data[i];
			glm::vec3 vel =p.vel;

        }

	
}


static bool CR_STATE alreadyInitialized = false;
static bool CR_STATE shouldStepFuckOff = false;



/*
NOTICE!!!!!!!!!
 
THE ORDER OF STEPS TAKEN IN THIS SWITCH WHHEN  WE HAVE TRIGGERED A REBUILD OF GAME DLL, IS COMPLETELTY ALL OVER THE PLACE, IT CAN TRIGGER
LOAD FAIL LOAD, BACK TO STEP, AND UNLOAD.. FORCING ME TO USE A BOOLEAN CALLED SHOULDSTEPFUCKOFF, NOTICE!!! UNLOAD CAN ALSO HINDER, FORCING ANOTHER REVERSE BOOL THERE!!!

*/
CR_EXPORT int cr_main(cr_plugin *ctx, cr_op operation){
	PassedStructuresDLL* psd = static_cast<PassedStructuresDLL*>(ctx->userdata);	
	
	ECS::Registry& reg = *psd->reg;
	EngineAPI& api = *psd->api;
	Input::State& state = *psd->state; 
	float& aspect = *psd->aspect;
	float& dt = *psd->dt;

	
		switch (operation) {
			case CR_LOAD:{
				printf("RELOADING DONE (OR FIRST LOAD)\n");
				if(ctx->failure != CR_BAD_IMAGE){
					if(!alreadyInitialized){
						init(reg, api);
						alreadyInitialized = true;
					}else{
						shouldStepFuckOff = false;
						printf("goo1");
						reg.createPool<Weapon>();
						printf("g22o3");
						std::ifstream is("save.txt");
						printf("goxxxx21.5");
						
						if (!is.is_open()) {
							printf("save.txt FAILED TO OPEN at CWD: %s\n",
								std::filesystem::current_path().string().c_str());
						}
						auto packets = std::move(Serde::deserializeFile(is));    				
						printf("oo");
						is.close();
						printf("goo");
						for(auto& pkt: packets){
							reg.deserializeComponent(pkt.id,pkt.sName,std::move(pkt.vars));
							printf("goo");
						}
							printf("go7");
					}
				}
				else{
					printf("bad img still ..\n");
				}
			
			}
			break;

			case CR_STEP:{
				if(ctx->failure != CR_BAD_IMAGE && !shouldStepFuckOff){
					update(aspect, dt, state ,reg, api);
				}
			}
			break;

			case CR_UNLOAD:{
				printf("UNLOAD\n");
				if(!shouldStepFuckOff){
					auto& wPool = reg.getPool<Weapon>();	
					int g{};
					std::ofstream os("save.txt"); // std::ios_base::app
					if (os.fail()){
						printf("%s", strerror(errno));						
					}

					os << std::fixed << std::setprecision(3);

					Serde::serializePool<Weapon>(os, reg);		
					os.close();

					reg.destroyPool<Weapon>();
					shouldStepFuckOff = true;
				}
				
					

			}
			break;

			case CR_CLOSE:{

			}
			break;
		}
	
    return 0;
}