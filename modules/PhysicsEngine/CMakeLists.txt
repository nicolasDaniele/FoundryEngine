add_library(PhysicsEngine
    SHARED
	src/Collisions3D.cpp
	src/PhysicsSystem.cpp
	src/PhysicsApi.cpp
	src/RigidbodyVolume.cpp
)

target_include_directories(PhysicsEngine PUBLIC include)

target_link_libraries(PhysicsEngine 
    PUBLIC
	Core
	EngineInterfaces
)

target_compile_definitions(PhysicsEngine PRIVATE PHYSICSENGINE_EXPORTS)