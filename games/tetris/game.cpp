
#include "game.hpp"
#include "serde.hpp"
#include "sydney_physics.hpp"
inline glm::quat eulerDeg(glm::vec3 deg){
	return glm::quat(glm::radians(deg));
}

void init(ECS::Registry& reg, EngineAPI& api, RuntimeData& rnt){
	reg.createPool<Weapon>();
	
	api.loadScene("level1.json");
	
	// api.loadModels({"dragon.glb","shibahu.glb","fps_character_animation_pack_ak-47.glb", "cube.glb", "sphere.glb"});
	
	
	
	// TransformInfo t = {.position = glm::vec3(2.0, -5.0, 0.0), .quatRot = {}, .scale = glm::vec3{ 3.0, 1.0, 4.0 } };
	// Collider c = {.offset = {0.0,0.8,0.0}, .broadRadius = 5.0f, .narrowShape = Collider::AABB, .narrowExtents = {3.0,1.2,4.0}};

	// int floor = reg.createEntity();
	
	// reg.add(floor, t, c);
	// SydX::Particle p{};
	// p.inverseMass = 0.0;
	// reg.add(floor,p);
	// api.attachModel("cube.glb", floor);

	// int women = reg.createEntity();
	
	// TransformInfo w = {.position = {0.0, 2.0, 0.0}, .quatRot = {}, .scale = glm::vec3{ 1.0f }};
	// reg.add(women, w);
	// api.attachModel("shibahu.glb", women);
	// api.setAnimation("Take 001", women);
	// reg.add(women, Collider{.offset = {0.0,0.8,0.0}, .broadRadius = 1.0f, .narrowShape = Collider::OBB, .narrowExtents = {1.0,0.75,0.25}});
	
	// reg.add(women, SydX::Particle{.inverseMass = 1.0f, .vel = {}, .acc = {0.0, -1.0, 0.0}, .damping = 0.99f});
	// int gun = reg.createEntity();
	// TransformInfo g = {.position = {0.0,5.0, -1.5},.quatRot = eulerDeg({0.0, 180.0, 0.0})};
	
	// reg.getPool<Weapon>().assign(gun,{gun,30});
	
	// reg.add(gun, g);
	// api.attachModel("fps_character_animation_pack_ak-47.glb", gun);
	// api.setAnimation("RIG_UE5_Comando_AK_Idle", gun);
	

    // api.setGameCamera(reg.createEntity());
	// reg.emplace<TransformInfo>(api.getGameCamera());
	// reg.emplace<Camera>(api.getGameCamera());
	// reg.add(api.getGameCamera(), Collider{.offset = {0.0, -0.2, 0.0}, .narrowShape = Collider::SPHERE});
	// reg.setParent(gun, api.getGameCamera());
	
	
	// reg.add(api.getGameCamera(), SydX::Particle{.inverseMass = 1.0f, .vel = {}, .acc = {0.0, -1.0, 0.0}, .damping = 0.99f, .bounciness = 0.2});
	
	// rnt.Entities[0]= floor;
	// rnt.Entities[1]= women;
	// rnt.Entities[2]= gun;
	


}
static std::array<int,20> CR_STATE children{};
static int CR_STATE count{};
void update(float aspect, float dt, Input::State &state, ECS::Registry& reg, EngineAPI& api)
{		

		Camera& camera = reg.getPool<Camera>().get(api.getGameCamera()); 
		SydX::Particle& camParticle =  reg.getPool<SydX::Particle>().get(api.getGameCamera()); 
		TransformInfo& cameraPos = reg.getPool<TransformInfo>().get(api.getGameCamera()); 

		updateCameraOrientation(camera,state,0.3,aspect);
		updatePlayerMovement(camParticle, cameraPos, camera, state, 2.0);
		//updateGameCamera()
		updateWeaponSystem(state, api, reg);
		



		if(!reg.getPool<TransformInfo>().dense.size()){
			return; 
		}//TEMOPARRY BARRIER, SOMETIMES UPDATE CALLS WHEN


	

		TransformInfo& gun = reg.getPool<TransformInfo>().get(reg.getPool<ECS::Hierarchic>().dense[0]);
		Collider& womenCollider = reg.getPool<Collider>().data[1];
		ECS::Entity womenID = reg.getPool<Collider>().dense[1];
		//reg.getPool<TransformInfo>().get(womenID).addRot({30.0,0.0f,0.0f},dt);

		Collider& camCollider = reg.getPool<Collider>().data[1];
		if(state.keyPressed(Input::Key::MouseLeft)){
			
			SydX::Particle p = {.inverseMass = 1.0f, .vel = 5.0f * camera.dir, .acc = {0.0, -4.0,0.0}, .damping = 0.99f}; // c++20 forever
			TransformInfo t = {.position = camera.eye + 3.0f*camera.dir, .scale = {0.2f,0.2f,0.2f}};
			int bull = reg.createEntity();
			reg.add(bull,t);
			reg.add(bull, p);
			reg.add(bull, Collider{.broadRadius = 0.5f, .narrowShape = Collider::SPHERE, .narrowRadius = 0.3});
			api.attachModel("cube.glb", bull);
		}

		gun.setPos({0.0,-1.55, 0.02});
		gun.setRot({0.0, 180.0, 0.0});		
		
}



void mainMenu(ECS::Registry& reg, GameMemory* mem,Input::State& state){

	/*
		so get the correct iD? where should it be stored 
		make some cube spin around 
		int id = mem->rnt.spinningCubeID;

		reg.getPool<TransformInfo>().get(id).addRot({30.0,0.0f,0.0f},dt);

	*/

	//load game
	if(state.keyPressed(Input::Key::Num1)){
		//api.loadScene(level1.txt)
		//pumpItAgain(save1.txt)// let dll manipulate whole mem blcok? it doesnt own it anyways
		//mem->rnt.currentScene = SaveData::Scene::SCENE_1;
		return;
	}
	//newgame
	if(state.keyPressed(Input::Key::Num2)){
		//mem->rnt.currentScene = mem->save.scene // maybe rnt?
		return;
	}


}
static bool CR_STATE alreadyInitialized = false;

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
	GameMemory* mem = reinterpret_cast<GameMemory*>(raw.data);
	
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
						/*
						api.loadScene("main_menu.txt")
						mem->scene == GameState::Scene::MAIN_MENU;
						*/



						init(reg, api,mem->rnt);
						alreadyInitialized = true;
					}
				}
				else{
					printf("bad img still ..\n");
				}

			}
			break;

			case CR_STEP:{
				
				if(ctx->failure != CR_BAD_IMAGE){
					
					
					/*
					switch(mem->save.scene){
						case: MAIN_MENU:{
							mainMenu(reg, mem, state);
							break;
						}
						case: LEVEL_1:{
							runGame();
							break;
						}				
						case: LEVEL_2:{
							runMainMenu();
							break;
						
						}		
						default:
							break;
					}
					
					
					*/
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