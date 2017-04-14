
add_sources(libopenage
	keyframe_container.cpp
	#tube_container_iterator.cpp
	tube_event.cpp
	tube_queue.cpp
	tube_object.cpp
	tube_objectlist.cpp
	tube_continuous.cpp
	tube_discrete.cpp
	tube.cpp
	value_container.cpp
)

add_subdirectory(test)
add_subdirectory(demo)
