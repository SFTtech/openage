add_sources(libopenage
    action.cpp
    binding_context.cpp
    binding.cpp
	event.cpp
	input_context.cpp
	input_manager.cpp
	tests.cpp
)

add_subdirectory("legacy")
add_subdirectory("controller")
