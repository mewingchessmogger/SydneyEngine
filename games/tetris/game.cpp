
#include "game.hpp"

using Particle = physics::Particle;
struct Bullet{
	int speed{};
	REFLECT_1(speed);
};

void init(ECS::Registry& reg, EngineAPI& api){
	api.loadModel("models/cube_gltf.glb");
	api.loadModel("models/dragon.glb");
	api.loadModel("models/shibahu.glb");
	api.loadModel("models/fps_character_animation_pack_ak-47.glb");
	int floor = reg.createEntity();
	Transform t{};
	t.position = glm::vec3(0.0, -1.0, 0.0);
	t.scale = glm::vec3(2.0,1.0,2.0);
	reg.add(floor, t);
	api.attachModel("models/cube_gltf.glb", floor);
	
	
	int women = reg.createEntity();
	
	Transform w{};
	w.position = glm::vec3(2.0, -1.5, 0.0);
	w.scale = {2.0f,2.0f,2.0f};
	reg.add(women, w);
	api.attachModel("models/shibahu.glb", women);
	api.setAnimation("Take 001", women);
		
	int gun = reg.createEntity();
	Transform g{};
	g.position = {1.0, -1.5,0.0};
	g.rotation = {0.0, 0.0,0.0};

	reg.add(gun, g);
	api.attachModel("models/fps_character_animation_pack_ak-47.glb", gun);
	api.setAnimation("RIG_UE5_Comando_AK_Reload", gun);
	


    api.setGameCamera(reg.createEntity());
	reg.emplace<Camera>(api.getGameCamera());
	/*animation #0 'RIG_UE5_Comando_AK_Equip      ', duration (in ticks): 1.833333, ticks per second: 1000.000000, it has 110 channels
animation #1 'RIG_UE5_Comando_AK_Aim_Fire   ', duration (in ticks): 0.666667, ticks per second: 1000.000000, it has 110 channels
animation #2 'RIG_UE5_Comando_AK_Fire       ', duration (in ticks): 0.666667, ticks per second: 1000.000000, it has 110 channels
animation #3 'RIG_UE5_Comando_AK_Hold       ', duration (in ticks): 0.750000, ticks per second: 1000.000000, it has 110 channels
animation #4 'RIG_UE5_Comando_AK_Idle       ', duration (in ticks): 2.500000, ticks per second: 1000.000000, it has 110 channels
animation #5 'RIG_UE5_Comando_AK_Idle_Aim   ', duration (in ticks): 2.500000, ticks per second: 1000.000000, it has 110 channels
animation #6 'RIG_UE5_Comando_AK_Reload     ', duration (in ticks): 4.583333, ticks per second: 1000.000000, it has 110 channels
animation #7 'RIG_UE5_Comando_AK_Walk       ', duration (in ticks): 1.000000, ticks per second: 1000.000000, it has 110 channels
animation #8 'RIG_UE5_Comando_AK_Walk_Aim   ', duration (in ticks): 1.000000, ticks per second: 1000.000000, it has 110 channels
animation #9 'RIG_UE5_Comando_AK__Run       ', duration (in ticks): 0.791667, ticks per second: 1000.000000, it has 110 channels
animation #10 'RIG_UE5_Comando_Natural_pose */

	// int woman = reg.createEntity();
	// t.position = glm::vec3(3.0, -1.0, 0.0);
	// t.scale = glm::vec3(3.0,3.0,3.0);

	// r.id = 2;
	// reg.add(woman,t, r);
}

void update(float aspect, float dt, Input::State &state, ECS::Registry& reg, EngineAPI& api)
{		

		Camera& camera = reg.getPool<Camera>().get(api.getGameCamera()); 
		updateGameCamera(camera, state, 0.3, aspect, dt);
		
		if (state.keyPressed(Input::Key::Jump) || state.keyPressed(Input::Key::LeftClick)){
			int dragon = reg.createEntity();

			Transform T{};
			glm::vec3 floorForward = glm::normalize(glm::vec3(camera.dir.x, 0.0f, camera.dir.z));
			glm::vec3 eyeFloor = glm::vec3(camera.eye.x,0.0,camera.eye.z);
			T.position = eyeFloor + floorForward;
			T.scale = {2.0f,2.0f,2.0f};
			T.rotation = {0.0,1.0,0.0};
			Particle p{};
			p.pos = eyeFloor;
			p.vel = floorForward * 2.0f;
			p.acc = {0.0,-4.0,0.0};
			p.damping = 0.5;
			p.inverseMass = 15;
			Renderable R = {};
			
			api.attachModel("models/dragon.glb", dragon);
			reg.add(dragon,T,p);



		}

		




		auto& PPool = reg.getPool<physics::Particle>();

        for (int i{}; i < PPool.count; i++){
            ECS::Entity e = PPool.dense[i];
            Particle& p = PPool.data[i];
			glm::vec3 vel =p.vel;
			
            // if ((vel.x *vel.x + vel.y* vel.y +vel.z* vel.z) < 0.1f) {
            //     reg.destroy(e);
        	// }
			// if (p.pos.y <= -1.0){
			// 	p.forceAccumulator = p.forceAccumulator + glm::vec3{0.0,4.0,0.0};
            //std::cout << "VELOCITY of entity " << (int)e << ": " <<  p.vel.x << "\n";
        }

	
}


static bool CR_STATE alreadyInitialized = false;
CR_EXPORT int cr_main(cr_plugin *ctx, cr_op operation){
	PassedStructuresDLL* psd = static_cast<PassedStructuresDLL*>(ctx->userdata);	
	int foo{}; 
	switch (operation) {
		case CR_LOAD:{
			if(!alreadyInitialized){
				init(*psd->reg, *psd->api);
				alreadyInitialized = true;
			}else{
				printf("Hmm weve already initted this time..!\n");
			}
		}
		break;

		case CR_STEP:{
			update(*psd->aspect, *psd->dt, *psd->state ,*psd->reg, *psd->api);
		}
		break;

		case CR_UNLOAD:{

		}
		break;

		case CR_CLOSE:{

		}
		break;
	}
    return 0;
}