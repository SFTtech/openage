add_sources(${PROJECT_NAME}
	tests.cpp
)

add_test_cpp(openage::util::tests::block_allocator "test functionality of the block allocator")
add_test_cpp(openage::util::tests::stack_allocator "test functionality of the stack allocator")
add_test_cpp(openage::util::tests::fixed_block_allocator "test functionality of the fixed block allocator")
add_test_cpp(openage::util::tests::fixed_stack_allocator "test functionality of the fixed stack allocator")
