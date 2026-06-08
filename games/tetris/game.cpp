#include "game.hpp"

using Particle = physics::Particle;

void Tetris::init(ECS::Registry& reg, GameContext& ctx){
	int floor = reg.createEntity();
	Renderable r{};
	r.meshID = 1;
	Transform t{};
	t.scale = {3.0, 0.2, 3.0};
	t.position = glm::vec3(0.0, -1.0, 0.0);
	reg.add(floor, t , r);
	
}

void Tetris::update(float aspect, float dt, Input::State &state, ECS::Registry& reg, GameContext& ctx)
{

		static float pitch = 0.0;
		static float yaw = 0.0;
		
		pitch += state.mouseDY * 0.3;
		yaw += state.mouseDX * 0.3;

		if(pitch > 89.0f)
			pitch =  89.0f;
		if(pitch < -89.0f)
			pitch = -89.0f;

		

		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);

		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		glm::vec3 cameraFront = glm::normalize(direction);

		
		static glm::vec3 eye  = glm::vec3(0.0f, 0.2f, -1.0f);
		glm::vec3 cameraUp     = glm::vec3(0.0f, 1.0f, 0.0f);
		float sensitivity = 1.0f;
		if (state.keyHeld(Input::Key::Forward)){
			eye += glm::vec3(cameraFront.x,0.0,cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::Backward)){
			eye -= glm::vec3(cameraFront.x,0.0,cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::Left)){
			eye -= glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;

		}
		if (state.keyHeld(Input::Key::Right)){
			eye += glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;
		}
		if (state.keyPressed(Input::Key::RightClick)){
			state.requestCursorVisible = !state.requestCursorVisible;
		}

		
		

		glm::mat4 view   = glm::lookAt(eye, cameraFront+eye, cameraUp);
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

		auto& cam0 = reg.getPool<Camera>().data[0];
		cam0.view = view;
		cam0.proj = proj;

		
		if (state.keyPressed(Input::Key::Jump) || state.keyPressed(Input::Key::LeftClick)){
			std::cout<< "ADDED OBJECT!!\n";
			int dragon = reg.createEntity();

			Transform T{};
			glm::vec3 floorForward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
			glm::vec3 eyeFloor = glm::vec3(eye.x,0.0,eye.z);
			T.position = eyeFloor+floorForward;
			T.scale = glm::vec3(2.0f);
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
