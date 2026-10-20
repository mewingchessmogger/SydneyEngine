
#include "game.hpp"
#include "sydney_physics.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
inline glm::quat eulerDeg(glm::vec3 deg){
	return glm::quat(glm::radians(deg));
}

void init(ECS::Registry& reg, EngineAPI& api, RuntimeData& rnt){
	reg.createPool<Weapon>();
	
	//api.loadScene("games/tetris/scenes/level1.json");
	
	api.loadModels({"dragon.glb","shibahu.glb","fps_character_animation_pack_ak-47.glb", "cube.glb", "sphere.glb"});
	/*
	cam, cam, floor,women, gun
	0,1,2,3,4
	*/
	TransformInfo t = {.position = glm::vec3(2.0, -5.0, 0.0), .quatRot = {}, .scale = glm::vec3{ 3.0, 1.0, 4.0 } };
	Collider c = {.offset = {0.0,0.8,0.0}, .broadRadius = 5.0f, .narrowShape = Collider::AABB, .narrowExtents = {3.0,1.2,4.0}};

	int floor = reg.createEntity();
	
	reg.add(floor, t, c);
	SydX::Particle p{};
	p.inverseMass = 0.0;
	reg.add(floor,p);
	api.attachModel("cube.glb", floor);

	int women = reg.createEntity();
	
	TransformInfo w = {.position = {0.0, 2.0, 0.0}, .quatRot = {}, .scale = glm::vec3{ 1.0f }};
	reg.add(women, w);
	api.attachModel("shibahu.glb", women);
	api.setAnimation("Take 001", women);
	reg.add(women, Collider{.offset = {0.0,0.8,0.0}, .broadRadius = 1.0f, .narrowShape = Collider::OBB, .narrowExtents = {1.0,0.75,0.25}});
	reg.add(women, SydX::Particle{.inverseMass = 1.0f, .vel = {}, .acc = {0.0, -1.0, 0.0}, .damping = 0.99f});



	int gun = reg.createEntity();
	TransformInfo g = {.position = {0.0, -1.57, 0.0},.quatRot = eulerDeg({0.0, 180.0, 0.0})};	
	reg.add(gun, g);
	api.attachModel("fps_character_animation_pack_ak-47.glb", gun);
	api.setAnimation("RIG_UE5_Comando_AK_Idle", gun);
	
	int camera = reg.createEntity();
    api.setGameCamera(camera);
	reg.add(camera, TransformInfo{.position = {0.0,0.0, 0.0}});
	reg.emplace<Camera>(camera);
	//reg.add(camera, Collider{.offset = {0.0, 0.0, 0.0}, .narrowShape = Collider::OBB, .narrowExtents = {0.20,0.20,0.20}});

	reg.setParent(gun, camera);
	
	int player = reg.createEntity();
	reg.emplace<TransformInfo>(player);
	reg.add(player, Collider{.offset = {0.0, -0.20, 0.0},.broadRadius = 1.1, .narrowShape = Collider::AABB, .narrowExtents = {0.25,0.75,0.25}});
	reg.setParent(camera, player);
	
	reg.add(player, SydX::Particle{.inverseMass = 1.0f, .vel = {}, .acc = {0.0, -1.0, 0.0}, .damping = 0.99f, .bounciness = 0.2});
	
	
	// gun.setPos({0.0,-1.57, -0.02});
	// gun.setRot({0.0, 180.0, 0.0});
	rnt.Entities[0] = gun;

}

void update(float aspect, float dt, Input::State &state, ECS::Registry& reg, EngineAPI& api, GameMemory* mem)
{		



		Camera& camera = reg.getPool<Camera>().get(api.getGameCamera()); 
		//SydX::Particle& camParticle =  reg.getPool<SydX::Particle>().get(api.getGameCamera()); 
		TransformInfo& cameraTrans = reg.getPool<TransformInfo>().get(api.getGameCamera()); 
		TransformInfo& playerTrans = reg.getPool<TransformInfo>().get(reg.getPool<ECS::Hierarchic>().get(api.getGameCamera()).parent);
		
		//*********************************** */
		
		float& pitch = camera.pitch;
		float& yaw = camera.yaw;

		// Store targets (these hold the raw mouse input)
		static float targetPitch = pitch;
		static float targetYaw = yaw;

		targetPitch += state.mouseDY * 0.3f;
		targetYaw += state.mouseDX * 0.3f;

		// Clamp the target, not just the actual pitch
		if(targetPitch > 89.0f) targetPitch = 89.0f;
		if(targetPitch < -89.0f) targetPitch = -89.0f;

		// Smoothly blend the actual camera rotation toward the target
		float lookSmoothness = 40.0f; // Higher = less input lag, Lower = heavier camera
		pitch = glm::mix(pitch, targetPitch, lookSmoothness * dt);
		yaw = glm::mix(yaw, targetYaw, lookSmoothness * dt);


		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 cameraUp = {0.0, 1.0, 0.0};

		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		glm::vec3 cameraDir = glm::normalize(direction);

		float sensitivity = 1.0f;

		float cameraYAxis{};
		

		glm::vec3 moveForward = glm::vec3(cameraDir.x, cameraYAxis, cameraDir.z) * dt;
		glm::vec3 moveRight = glm::normalize(glm::cross(cameraDir, cameraUp)) * dt;

		if (state.keyHeld(Input::Key::C)) {
			playerTrans.addPos(moveForward);
		}
		if (state.keyHeld(Input::Key::X)) {
			playerTrans.addPos(-moveForward);
		}
		if (state.keyHeld(Input::Key::Z)) {
			playerTrans.addPos(-moveRight);
		}
		if (state.keyHeld(Input::Key::V)) {
			playerTrans.addPos(moveRight);
		}

		
		cameraTrans.quatRot = glm::quatLookAt(cameraDir,cameraUp);
		cameraTrans.dirty = true;
		
		camera.view = glm::lookAt(playerTrans.position, cameraDir + playerTrans.position, cameraUp);
		camera.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		camera.dir =cameraDir;


		//updateGameCamera(camera, cameraPos, state, 0.3, aspect, dt);
		updateWeaponSystem(state, api, reg, mem->rnt);\

		


		//api.bus.handleCollisionSubscriptions();

		for(int i{}; i < api.bus.subscribedEntities.size();){
			Subscribed& sub = api.bus.subscribedEntities[i];
			int e = sub.ent; 
			bool swappedAndPopped = false;

			for(auto& collision: api.bus.collisions){
				if(e == collision.self){
					mem->rnt.callbacks[sub.vTableSlot](collision.self, collision.other, &reg);

					//sub.callback(collision.self, collision.other);
					reg.destroy(e);
					api.bus.subscribedEntities[i] = api.bus.subscribedEntities.back();
					api.bus.subscribedEntities.pop_back();
					swappedAndPopped = true;
					break;
				}
			}
			if(!swappedAndPopped){
				i++;
			}
		}
		api.bus.collisions.clear();


		if(!reg.getPool<TransformInfo>().dense.size()){
			return; 
		}//TEMOPARRY BARRIER, SOMETIMES UPDATE CALLS WHEN

		TransformInfo& gun = reg.getPool<TransformInfo>().get(reg.getPool<ECS::Hierarchic>().dense[0]);
		Collider& womenCollider = reg.getPool<Collider>().data[1];
		ECS::Entity womenID = reg.getPool<Collider>().dense[1];
		//reg.getPool<TransformInfo>().get(womenID).addRot({30.0,0.0f,0.0f},dt);

		Collider& camCollider = reg.getPool<Collider>().data[1];

		if(state.keyPressed(Input::Key::MouseLeft)){
			int bullet = reg.createEntity();
			if((bullet % 2) == 0){
				api.bus.subscribedEntities.push_back({bullet, 0});
			}else{
				api.bus.subscribedEntities.push_back({bullet, 1});
			}
			reg.add(bullet,TransformInfo{.position = playerTrans.position + 3.0f*camera.dir, .scale = {0.2f,0.2f,0.2f}});
			reg.add(bullet, SydX::Particle{.inverseMass = 1.0f, .vel = 5.0f * camera.dir, .acc = {0.0, -4.0,0.0}, .damping = 0.99f});
			reg.add(bullet, Collider{.broadRadius = 0.5f, .narrowShape = Collider::SPHERE, .narrowRadius = 0.3});
			api.attachModel("cube.glb", bullet);
		}

				
		
}



void mainMenu(ECS::Registry& reg, GameMemory* mem,Input::State& state){

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
static bool CR_STATE firstBootup = false;


/*
NOTICE!!!!!!!!!


CR_LOAD WILL EXECTUE FIRSTLY FIRST STARTUP, BUT W	HEN HOT RELOAD(ME REBUILD GAME) HAPPENS, CR_LOAD MIGHT NOT HAPPEN FIRST, DUE TO THIS AFTER HOT RELOAD,
MAKE SURE CR_STEP CASE IS NEVER INVOKED UNTIL NEXT CR_LOAD IS INVOKED AND  MANAGES TO DESERIALIZE/CREATE POOLS, ALSO DUE TO SPEED, IT
 CAN LOAD, FAIL, STEP, UNLOAD MULTIPLE TIMES BECAUES REBULILD STILL LOCKING DLL/PDB

 ALSO WEIRD QUIRK!!! INIT AND UPDATE CAN FOLLOW EACHOTHER INSIDE THE SAME FRAME!!! DONT THINK STEP WILL EXECUTE NEXT FRAME!! IT CAN EXEUCTE DIRECTLY AFTER INIT!!!;
  EG CR_MAIN CAN BE CALLED TWICE WITHIN THE SAME UPDATE FUNC
 

  OBSERVER that cr load will hpappne first iteration and then immeidately after cr step will happen aswell
*/
void print1(int e1, int e2, ECS::Registry* reg){
	printf("hello!\n");
}
void print2(int e1, int e2, ECS::Registry* reg){
	
	printf("goodbye!\n");
}


CR_EXPORT int cr_main(cr_plugin *ctx, cr_op operation){
	PassedStructuresDLL* psd = static_cast<PassedStructuresDLL*>(ctx->userdata);	
	ECS::Registry& reg = *psd->reg;
	EngineAPI& api = *psd->api;
	Input::State& state = *psd->state; 
	float& aspect = *psd->aspect;
	float& dt = *psd->dt;
	RawMemory& raw = *psd->mem;
	GameMemory* mem = reinterpret_cast<GameMemory*>(raw.data);

	switch (operation) 
		{
			case CR_LOAD:
			{
				mem->rnt.callbacks = {print1,print2};
				printf("RELOADING DONE (OR FIRST LOAD)\n");
				if(ctx->failure != CR_BAD_IMAGE){

					/*
					remap all function poointers 
					*/
					if(!alreadyInitialized){
						/*
						api.loadScene("main_menu.txt")
						mem->scene == GameState::Scene::MAIN_MENU;
						*/

						init(reg, api,mem->rnt);
						alreadyInitialized = true;
						firstBootup = true;
					}
				}
				else{
					printf("bad img still ..\n");
				}

			}
			break;

			case CR_STEP:{
				
				if(ctx->failure != CR_BAD_IMAGE){
					if(firstBootup){
						firstBootup = false;
						return 0;
					}
					
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
					update(aspect, dt, state ,reg, api, mem);
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