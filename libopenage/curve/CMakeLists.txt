
add_sources(libopenage
	internal/keyframe_container.cpp
	internal/value_container.cpp
	queue.cpp
	continuous.cpp
	discrete.cpp
	events.cpp
	iterator.cpp
	map.cpp
	map_filter_iterator.cpp
	queue.cpp
	queue.cpp
	queue_filter_iterator.cpp
)

add_subdirectory(internal)
add_subdirectory(test)
add_subdirectory(demo)
