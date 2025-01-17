add_sources(libopenage
    cost_field.cpp
	definitions.cpp
	field_cache.cpp
    flow_field.cpp
	grid.cpp
	integration_field.cpp
    integrator.cpp
	path.cpp
	pathfinder.cpp
	portal.cpp
	sector.cpp
	tests.cpp
    types.cpp
)

add_subdirectory("demo")
add_subdirectory("legacy")
