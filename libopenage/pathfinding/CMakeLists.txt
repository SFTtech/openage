add_sources(libopenage
	a_star.cpp
    cost_field.cpp
	definitions.cpp
    flow_field.cpp
	grid.cpp
	heuristics.cpp
	integration_field.cpp
    integrator.cpp
	path.cpp
	portal.cpp
	sector.cpp
	tests.cpp
    types.cpp
)

add_subdirectory("demo")
