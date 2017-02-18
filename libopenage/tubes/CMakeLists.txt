
add_sources(libopenage
	base.cpp
	container_iterator.cpp
	event.cpp
	eventlist.cpp
	filter_iterator.cpp
	object.cpp
	simple_continuous.cpp
	simple_discrete.cpp
	simple_type.cpp
	tube.cpp
	unordered_map.cpp
)

add_subdirectory(test)
add_subdirectory(demo)
