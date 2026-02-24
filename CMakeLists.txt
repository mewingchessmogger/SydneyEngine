cmake_minimum_required(VERSION 4.00)
project(SydneyEngine)
set(CMAKE_CXX_STANDARD 17)
find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)
add_subdirectory(library)

add_library(glm_vulkan_defines INTERFACE)
target_compile_definitions(glm_vulkan_defines INTERFACE 
    GLM_FORCE_DEPTH_ZERO_TO_ONE 
    GLM_FORCE_RADIANS
    GLM_ENABLE_EXPERIMENTAL
)

if(MSVC)
    # /MP4 limits compilation to 4 processes
    # /Gm- is required for multi-processor builds to work correctly
    add_compile_options(/MP4 /Gm-)
    
    # Speed up the linker by using incremental linking and fast pdb generation
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG:FASTLINK /INCREMENTAL")
endif()

file(GLOB_RECURSE SOURCE_FILES 
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
    "main.cpp"
)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES})
#target_link_libraries(${PROJECT_NAME} glfw)


target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glm_vulkan_defines
    Vulkan::Vulkan
    Vulkan::shaderc_combined
    glfw 
    imgui_lib 
    vkbootstrap
    vma
    tinygltf
    stb
    glm
)