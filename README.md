A refactor of [previous project](https://github.com/mewingchessmogger/vulkan_renderer_cmake)  focusing on way better code architecture and more reusable abstractions, currently only rendering with one pipeline showing normals of models. 
The libraries im using are: tinygltf, glm, vma, vkbootstrap(ive done [oovervoord's tutorial](https://vulkan-tutorial.com/) before so im not gonna search for queues again!),imgui and glfw


Currently can spawn stanford dragons by pressing space on keyboard. Basic camera movement with WASD.The model can be changed by entering the updateGame function and changing below pictures variable "MeshID" to 0 or 1 spawing currently stanford dragon or a cube.

<img width="1121" height="481" alt="image" src="https://github.com/user-attachments/assets/c16678b1-4984-44f4-b99e-a638e748d0d0" />

This should be the result
<img width="1580" height="1072" alt="image" src="https://github.com/user-attachments/assets/17f637d9-b8aa-4f77-9e90-1d4fd930b95b" />
