#include "engine.hpp"
#include "hash_model.hpp"
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
 
// #define  GLM_FORCE_RADIANS
// #define  GLM_ENABLE_EXPERIMENTAL

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <glm/gtx/projection.hpp>
/*
"../../../../models/dragon.glb");
    ast.addUploadRequest("../../../../models/cube_gltf.glb"
	*/
void Engine::updateGame(Scene& scn, float aspect, Input::State &state,ModelStorage& storage){
		static std::vector<float> timers{};
		static float timer = 0.0f;
		timer += 0.01f; 
		
		
		//std::cout << state.mouseDX << "!!\n";
		
		

		static float pitch = 0.0;
		static float yaw = 0.0;

		pitch += state.mouseDY * 0.5;
		yaw += state.mouseDX * 0.5;

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
			eye += glm::vec3(cameraFront.x,0.0,cameraFront.z) * state.deltaTime;
		}
		if (state.keyHeld(Input::Key::Backward)){
			eye -= glm::vec3(cameraFront.x,0.0,cameraFront.z) * state.deltaTime;
		}
		if (state.keyHeld(Input::Key::Left)){
			eye -= glm::normalize(glm::cross(cameraFront, cameraUp)) * state.deltaTime;

		}
		if (state.keyHeld(Input::Key::Right)){
			eye += glm::normalize(glm::cross(cameraFront, cameraUp)) * state.deltaTime;
		}


		
		

		glm::mat4 view   = glm::lookAt(eye, cameraFront+eye, cameraUp);
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);


		scn.data = {view, proj};

		
		if (state.keyPressed(Input::Key::Jump)){
			std::cout<< "ADDED OBJECT!!\n";
			Scene::GameObject obj{};
			
			glm::vec3 floorForward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
			glm::vec3 eyeFloor = glm::vec3(eye.x,0.0,eye.z);
			
			obj.model = glm::translate(glm::mat4(1.0f),eyeFloor+floorForward);
			obj.model = glm::scale(obj.model,glm::vec3(0.5));
			obj.meshID = 0;
			scn.gameObjects.push_back(obj);

		}

		// for(auto& object : scn.gameObjects){
		// 	if(object.meshID = storage.getModelID("../../../../models/dragon.glb").modelID){
		// 		object.model = glm::rotate(object.model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// 	}
		// }

		
		
}