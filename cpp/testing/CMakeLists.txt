# testing subsystem cmake file.

# add source files to binary
add_sources(${PROJECT_NAME}
	datastructure.cpp
	testing.cpp
)

# register each available c++ test
add_test_cpp(tests::doubly_linked_list "test functionality of the circular linked list")
