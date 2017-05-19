
add_sources(libopenage
	internal/keyframe_container.cpp
	internal/value_container.cpp
	queue.cpp
	continuous.cpp
	discrete.cpp
	curve.cpp
)

add_subdirectory(test)
add_subdirectory(demo)
