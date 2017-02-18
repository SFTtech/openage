link_libraries(tube)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")

add_sources(libopenage
	test_container.cpp
	test_tube_types.cpp
	test_serialization.cpp
)
