cmake_minimum_required(VERSION 3.20)

project(EngineSolution)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Third Party
add_subdirectory(third_party/glfw)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE third_party/stb)

add_library(glad STATIC third_party/glad/src/glad.c)
    target_include_directories(glad PUBLIC third_party/glad/include
)
set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Add Modules
add_subdirectory(modules/EngineInterfaces)
add_subdirectory(modules/Core)
add_subdirectory(modules/GraphicsEngine)
add_subdirectory(modules/PhysicsEngine)
add_subdirectory(modules/Debugger)

# App
add_subdirectory(app/FoundryEngine)