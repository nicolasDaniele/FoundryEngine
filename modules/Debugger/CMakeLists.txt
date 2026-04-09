add_library(Debugger
    src/DebugRenderer.cpp
)

target_include_directories(Debugger PUBLIC include)

target_include_directories(Debugger PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
)

target_link_libraries(Debugger
    PUBLIC
        EngineInterfaces
	Core
	glad
)