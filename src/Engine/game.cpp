#include "game.hpp"
#include "fysik_motor.hpp"
#include "engine_components.hpp"
/*
"../../../../models/dragon.glb");
    ast.addUploadRequest("../../../../models/cube_gltf.glb"
	*/


void updateGame(Scene& scn, float aspect, float dt, Input::State &state, ModelStorage& storage, ECS::Registry& reg){
		
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

		
		scn.data = {view, proj};


		
		if (state.keyPressed(Input::Key::Jump) || state.keyPressed(Input::Key::LeftClick)){
			std::cout<< "ADDED OBJECT!!\n";
			int dragon = reg.createEntity();

			Transform T{};
			glm::vec3 floorForward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
			glm::vec3 eyeFloor = glm::vec3(eye.x,0.0,eye.z);
			T.position = eyeFloor+floorForward;
			T.scale = glm::vec3(2.0f);
			
			Particle p{};
			p.pos = eyeFloor;
			p.vel = floorForward * 2.0f;
			
			p.damping = 0.5;
			p.inverseMass = 10;
			Renderable R = Renderable{0};
			
			reg.add(dragon,T,p,R);

			// obj.model = glm::translate(glm::mat4(1.0f),eyeFloor+floorForward);
			// obj.model = glm::scale(obj.model,glm::vec3(2.0f));
			// obj.meshID = 0; 
			

			// scn.gameObjects.push_back(obj);

		}

		// for(auto& object : scn.gameObjects){
		// 	if(object.meshID == storage.getModelID("../../../../models/dragon.glb").modelID){
		// 		object.model = glm::rotate(object.model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// 	}
		// }

		
		
}

void initGame(Scene& scn, ECS::Registry& reg){
	int floor = reg.createEntity();

	
	Renderable r{};
	r.meshID = 1;
	Transform t{};
	t.scale = {3.0, 0.2, 3.0};
	t.position = glm::vec3(0.0, -1.0, 0.0);
	reg.add(floor, t , r);

	
	
	
	
	
	// floor.meshID = 1;
    // floor.model = glm::translate(floor.model, glm::vec3(0.0, -1.0, 0.0));
	// floor.model = glm::scale(floor.model, glm::vec3(3.0, 0.2, 3.0));
	
	
	// scn.gameObjects.push_back(floor);
	
}