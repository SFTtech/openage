add_sources(${PROJECT_NAME}
	allocator_tests.cpp
	color.cpp
	dir.cpp
	error.cpp
	externalprofiler.cpp
	file.cpp
	fds.cpp
	fps.cpp
	misc.cpp
	opengl.cpp
	strings.cpp
	timer.cpp
	unicode.cpp
)

add_test_cpp(openage::util::tests::block_allocator "test functionality of the block allocator")
add_test_cpp(openage::util::tests::stack_allocator "test functionality of the stack allocator")
add_test_cpp(openage::util::tests::fixed_block_allocator "test functionality of the fixed_block_allocator")
add_test_cpp(openage::util::tests::fixed_stack_allocator "test functionality of the fixed_block_allocator")