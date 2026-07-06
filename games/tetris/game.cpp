#include "game.hpp"

using Particle = physics::Particle;
struct Bullet{
	int speed{};
	REFLECT_1(speed);
};

void Tetris::init(ECS::Registry& reg, GameContext& ctx){
	int floor = reg.createEntity();
	int bullet = reg.createEntity();
	Renderable r{};
	r.meshID = 1;
	Transform t{};
	t.position = glm::vec3(0.0, -1.0, 0.0);
	reg.add(floor, t , r);
	reg.createPool<Bullet>();
	reg.emplace<Bullet>(bullet);
}

void Tetris::update(float aspect, float dt, Input::State &state, ECS::Registry& reg, GameContext& ctx)
{

		auto& cam0 = reg.getPool<Camera>().data[0];
		
		
		if (state.keyPressed(Input::Key::Jump) || state.keyPressed(Input::Key::LeftClick)){
			std::cout<< "ADDED OBJECT!!\n";
			int dragon = reg.createEntity();

			Transform T{};
			glm::vec3 floorForward = glm::normalize(glm::vec3(cam0.dir.x, 0.0f, cam0.dir.z));
			glm::vec3 eyeFloor = glm::vec3(cam0.eye.x,0.0,cam0.eye.z);
			T.position = eyeFloor+floorForward;
			
			T.rotation = {0.0,1.0,0.0};
			Particle p{};
			p.pos = eyeFloor;
			p.vel = floorForward * 2.0f;
			p.acc = {0.0,-4.0,0.0};
			p.damping = 0.5;
			p.inverseMass = 10;
			Renderable R = Renderable{0};
			
			reg.add(dragon,T,p,R);


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
