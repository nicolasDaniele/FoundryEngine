add_library(Core
    src/Geometry3D.cpp
    src/Matrices.cpp
    src/Utils.cpp
    src/Vectors.cpp
)

target_include_directories(Core PUBLIC include)

target_link_libraries(Core PUBLIC EngineInterfaces)