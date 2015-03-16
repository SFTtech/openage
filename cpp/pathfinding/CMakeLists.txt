add_sources(${PROJECT_NAME}
	a_star.cpp
	heuristics.cpp
	path.cpp
	path_utils.cpp
	tests.cpp
)

add_test_cpp(openage::path::tests::path_node "test functionality of the path node")
