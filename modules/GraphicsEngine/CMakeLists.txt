add_library(GraphicsEngine
    SHARED
	src/Camera.cpp
	src/GraphicsApi.cpp
	src/MeshBuffer.cpp
	src/MeshFactory.cpp
	src/MeshRenderer.cpp
	src/ShaderLoader.cpp
	src/TextureLoader.cpp
)

target_include_directories(GraphicsEngine PUBLIC include)

target_link_libraries(GraphicsEngine 
     PUBLIC
	Core
	EngineInterfaces
	glfw
	stb
     PRIVATE
	glad
)

target_compile_definitions(GraphicsEngine PRIVATE GRAPHICSENGINE_EXPORTS)