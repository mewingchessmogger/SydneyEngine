#include "engine.hpp"



void Engine::updateGame(Scene& scn, float aspect, Input::State &state){
		static std::vector<float> timers{};
		static float timer = 0.0f;
		timer += 0.01f; 


		if (state.keyPressed(Input::Key::Jump)){
			std::cout<< "JUMP!!\n";
			
		}
		if (state.keyReleased(Input::Key::Jump)){
			std::cout<< "JUMP DONE!!\n";

		}
		
		if (state.keyPressed(Input::Key::Forward)){
			std::cout<< "ADDED OBJECT!!\n";
			Scene::GameObject obj{};
			scn.gameObjects.push_back(obj);
			timers.push_back(timer);
		}


		glm::vec3 eye    = glm::vec3(0.5f, 0.0f, 1.0f);
		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 view   = glm::lookAt(eye, target, up);
		view = glm::lookAt(eye, target, scn.camera.up);
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

		

		scn.data = {view, proj};


		for(uint32_t i{}; i < scn.gameObjects.size(); i++){
			float j = float(i + 1);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::scale(model,glm::vec3(2.0));
			model = glm::rotate(model, (timer - timers[i]), glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis
			model = glm::rotate(model, (timer - timers[i]), glm::vec3(0.0f, 0.0f, 1.0f)); // Z-axis
			scn.gameObjects[i].model = model;
		}

		
		
}