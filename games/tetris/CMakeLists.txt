cmake_minimum_required(VERSION 4.00)

project(TetrisDLL)
set(CMAKE_BUILD_PARALLEL_LEVEL 8)
set(CMAKE_CACHE_VERSION 2)
set(CMAKE_CXX_STANDARD 23)
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
    "game.cpp"
    "game.hpp"
    "systems_game.cpp"
)

add_library(${PROJECT_NAME} SHARED ${DLL_SOURCES})
# 4. Target-specific configurations (MUST come AFTER add_library)

# if(MSVC)
#     target_compile_options(${PROJECT_NAME} PRIVATE 
#         /Bt          # Measures time spent in C1/C1XX (Frontend: parsing & headers) and C2 (Backend: codegen)
#         /d2cgsummary # Prints detailed code-generation and optimization pass timings
#     )
#     target_link_options(${PROJECT_NAME} PRIVATE 
#         /TIME        # Prints detailed linker pass timings (LTCG, symbol resolution, PDB emission)
#     )
# endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE pch_game.hpp)

# 4. Only pull the shared engine include headers
target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../library/"
    
)

# 5. Link GLM for the camera math
#target_link_libraries(${PROJECT_NAME} PRIVATE)


