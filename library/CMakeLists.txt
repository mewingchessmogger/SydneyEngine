add_subdirectory(glfw)
add_subdirectory(glm)

#Relative paths in add_library and add_executable are automatically interpreted as relative to the current directory.
add_library(imgui_lib STATIC 
    imgui/imgui.cpp 
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_vulkan.cpp
)

target_link_libraries(imgui_lib PRIVATE 
    glfw 
    Vulkan::Vulkan
)

target_include_directories(imgui_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)

#  ASSIMP
# Define the library as an imported target
add_library(assimp_lib SHARED IMPORTED GLOBAL) #FUCK THIS FUCKING GLOBAL MISSING DUMB FCKING CMAKE

# Set the binary locations
set_target_properties(assimp_lib PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/assimp/assimp-vc145-mt.dll"
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/assimp/assimp-vc145-mt.lib"
)

# Tell CMake where to find the headers for anyone linking to this
target_include_directories(assimp_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/assimp/include")



# --- Vulkan Memory Allocator (VMA) ---
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vma")


add_library(json INTERFACE)
target_include_directories(json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/json")


# --- STB (For standalone image/font usage) ---
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb_image")

target_link_libraries(stb INTERFACE 
    json
)

# --- TinyGLTF (Includes json.hpp and stb_image) ---
add_library(tinygltf INTERFACE)

target_include_directories(tinygltf INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/tinygltf")

add_library(vkbootstrap STATIC 
    vkbootstrap/VkBootstrap.cpp 
    vkbootstrap/VkBootstrap.h
    vkbootstrap/VkBootstrapDispatch.h

)

target_link_libraries(vkbootstrap PRIVATE  
    Vulkan::Vulkan
)



target_include_directories(vkbootstrap PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vkbootstrap")


#add_library(INTERFACE)
#target_include_directories(json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

