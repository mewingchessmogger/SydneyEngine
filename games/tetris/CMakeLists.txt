cmake_minimum_required(VERSION 4.00)
project(TetrisDLL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Force the output straight into your game runtime folder
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../tetris")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../tetris")

# 2. Match your engine's precise GLM layout flags
add_compile_definitions(
    GLM_FORCE_DEPTH_ZERO_TO_ONE 
    GLM_FORCE_RADIANS
    GLM_ENABLE_EXPERIMENTAL
)

# 3. Compile the local files into the DLL
set(DLL_SOURCES 
    "camera.hpp"
    "game.cpp"
    "game.hpp"
    "systems_game.cpp"
    )

add_library(${PROJECT_NAME} SHARED ${DLL_SOURCES})

if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE /INCREMENTAL:NO)
endif()

# 4. Only pull the shared engine include headers
target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../library"
    
)

# 5. Link GLM for the camera math
#target_link_libraries(${PROJECT_NAME} PRIVATE)


