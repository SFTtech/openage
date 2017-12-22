
add_sources(libopenage
	internal/keyframe_container.cpp
	internal/value_container.cpp
	queue.cpp
	continuous.cpp
	discrete.cpp
	iterator.cpp
	map.cpp
	map_filter_iterator.cpp
	queue.cpp
	queue_filter_iterator.cpp
)

pxdgen(
	continuous.h
	curve.h
	discrete.h
	iterator.h
	map.h
	map_filter_iterator.h
	queue.h
	queue_filter_iterator.h
)

add_subdirectory(events)
add_subdirectory(internal)
add_subdirectory(test)
add_subdirectory(demo)
