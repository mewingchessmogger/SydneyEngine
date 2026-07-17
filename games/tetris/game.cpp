#include "game.hpp"

using Particle = physics::Particle;
struct Bullet{
	int speed{};
	REFLECT_1(speed);
};

void Tetris::init(ECS::Registry& reg, EngineAPI& api){
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
	w.position = glm::vec3(1.0, 0.0, 0.0);
	w.scale = glm::vec3(1.0,1.0,1.0);
	reg.add(women, w);
	api.attachModel("models/fps_character_animation_pack_ak-47.glb", women);
	api.setAnimation("RIG_UE5_Comando_AK_Reload", women);
	
    api.setGameCamera(reg.createEntity());
	reg.emplace<Camera>(api.getGameCamera());

	// int woman = reg.createEntity();
	// t.position = glm::vec3(3.0, -1.0, 0.0);
	// t.scale = glm::vec3(3.0,3.0,3.0);

	// r.id = 2;
	// reg.add(woman,t, r);
}

void Tetris::update(float aspect, float dt, Input::State &state, ECS::Registry& reg, EngineAPI& api)
{		

		Camera& camera = reg.getPool<Camera>().get(api.getGameCamera()); 
		updateGameCamera(camera, state, 0.3, aspect, dt);
		
		if (state.keyPressed(Input::Key::Jump) || state.keyPressed(Input::Key::LeftClick)){
			int dragon = reg.createEntity();

			Transform T{};
			glm::vec3 floorForward = glm::normalize(glm::vec3(camera.dir.x, 0.0f, camera.dir.z));
			glm::vec3 eyeFloor = glm::vec3(camera.eye.x,0.0,camera.eye.z);
			T.position = eyeFloor + floorForward;
			
			T.rotation = {0.0,1.0,0.0};
			Particle p{};
			p.pos = eyeFloor;
			p.vel = floorForward * 2.0f;
			p.acc = {0.0,-4.0,0.0};
			p.damping = 0.5;
			p.inverseMass = 10;
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
