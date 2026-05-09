add_executable(FoundryEngine
    App.cpp
)

set(ENGINE_MODULES GraphicsEngine PhysicsEngine)

target_link_libraries(FoundryEngine
    PRIVATE
	${ENGINE_MODULES}
	Debugger
	Core
	glad
)

foreach(MODULE ${ENGINE_MODULES})
    add_custom_command(TARGET FoundryEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${MODULE}>
            $<TARGET_FILE_DIR:FoundryEngine>
    )
endforeach()

add_custom_command(TARGET FoundryEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:FoundryEngine>/Assets
)