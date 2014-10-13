# testing subsystem cmake file.

# add source files to binary
add_sources(${PROJECT_NAME}
	datastructure.cpp
	testing.cpp
	term.cpp
	engine_test.cpp
)

# register each available c++ test
add_test_cpp(tests::term0 "prints a few test lines to a buffer, and draws it to stdout")
add_test_cpp(tests::term1 "opens an interactive terminal with the user's shell")
add_test_cpp(tests::doubly_linked_list "test functionality of the circular linked list")
