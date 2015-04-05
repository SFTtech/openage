add_sources(${PROJECT_NAME}
	rng.cpp
	global_rng.cpp
	rng_tests.cpp
)

add_test_cpp(openage::rng::tests::rng_tests "test functionality of random number generator")
