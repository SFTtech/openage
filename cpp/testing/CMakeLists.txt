# testing subsystem cmake file.

# add source files to binary
add_sources(${PROJECT_NAME}
	datastructure.cpp
	testing.cpp
	term.cpp
	engine_test.cpp
)


# register each available c++ test
# name, description, function name, is_interactive
add_test_cpp(
	${PROJECT_NAME}
	"term_0"
	"prints a few test lines to a buffer, and draws it to stdout."
	"term0"
	"false"
)

add_test_cpp(
	${PROJECT_NAME}
	"term_1"
	"opens an interactive terminal with the user's shell."
	"term1"
	"true"
)

add_test_cpp(
	${PROJECT_NAME}
	"datastructure_linked_list"
	"test functionality of the circular linked list."
	"doubly_linked_list"
	"false"
)


# then generate the test registration file
register_tests_cpp(${PROJECT_NAME} register.cpp.in register.cpp)
