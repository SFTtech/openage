add_sources(libopenage
	base_curve.cpp
	continuous.cpp
	discrete.cpp
	discrete_mod.cpp
	interpolated.cpp
	keyframe.cpp
	keyframe_container.cpp
	segmented.cpp
)

add_subdirectory("container")
add_subdirectory("tests")
