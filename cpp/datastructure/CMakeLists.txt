add_sources(${PROJECT_NAME}
	tests.cpp
)

add_test_cpp(openage::datastructure::tests::doubly_linked_list "test functionality of the circular linked list")
