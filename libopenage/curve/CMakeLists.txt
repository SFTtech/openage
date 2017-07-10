
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
	queue_filter_iterator.cpp
	trigger.cpp
)

pxdgen(
	continuous.h
	curve.h
	discrete.h
	events.h
	iterator.h
	map.h
	map_filter_iterator.h
	queue.h
	queue_filter_iterator.h
	trigger.h
)


add_subdirectory(test)
add_subdirectory(demo)
