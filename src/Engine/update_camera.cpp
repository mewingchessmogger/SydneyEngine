#include "engine.hpp"
void Engine::updateCamera(Camera& cam,EngineMode context ,float sens)
{
        float aspect = plt.aspectRatio;
		float dt = plt.deltaTime;
		Input::State &state = plt.inputState;

        float& pitch = cam.pitch;
        float& yaw = cam.yaw;
        
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
		
		glm::vec3& eye = cam.eye;
		glm::vec3& cameraUp = cam.up;

	
		float sensitivity = 1.0f;

		float cameraYAxis{};
		if(context == EngineMode::EDITOR){
			cameraYAxis = cameraFront.y;
		}



		if (state.keyHeld(Input::Key::W)){
			eye += glm::vec3(cameraFront.x, cameraYAxis, cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::S)){
			eye -= glm::vec3(cameraFront.x, cameraYAxis, cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::A)){
			eye -= glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;

		}
		if (state.keyHeld(Input::Key::D)){
			eye += glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;
		}
		
		
		

		cam.view = glm::lookAt(eye, cameraFront+eye, cameraUp);
		cam.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		cam.dir = cameraFront;
}