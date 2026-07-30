cmake_minimum_required(VERSION 4.00)
# set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
# set(CMAKE_CXX_MODULE_STD 1)
project(SydneyEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

# add_library(VulkanHppModule)

# target_sources(VulkanHppModule
#     PUBLIC
#         FILE_SET CXX_MODULES 
#         BASE_DIRS
#             "${Vulkan_INCLUDE_DIRS}/vulkan"
#         FILES 
#             "${Vulkan_INCLUDE_DIRS}/vulkan/vulkan.cppm"

# )
# target_link_libraries(VulkanHppModule PUBLIC Vulkan::Vulkan)

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

# 2. Define the path to the DLL
target_precompile_headers(${PROJECT_NAME} PRIVATE include/pch.hpp)

target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    #VulkanHppModule
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
    assimp_lib
    cr
)
set(ASSIMP_DLL "${CMAKE_CURRENT_SOURCE_DIR}/library/assimp/assimp-vc145-mt.dll")

# 3. Use add_custom_command to handle the copy
add_custom_command(TARGET SydneyEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${ASSIMP_DLL}"
    $<TARGET_FILE_DIR:SydneyEngine>
)
