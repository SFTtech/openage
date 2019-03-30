add_sources(libopenage
	base_curve.cpp
	continuous.cpp
	curve.cpp
	discrete.cpp
	interpolated.cpp
	iterator.cpp
	keyframe.cpp
	keyframe_container.cpp
	map.cpp
	map_filter_iterator.cpp
	queue.cpp
	queue_filter_iterator.cpp
	segmented.cpp
)

add_subdirectory("tests")
