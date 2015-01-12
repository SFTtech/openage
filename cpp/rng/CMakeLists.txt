add_sources(${PROJECT_NAME}
	rng.cpp
	global_rng.cpp
	rng_tests.cpp
)

#[[Some basic distribution and uniformity tests are applied to the rng as a basic sanity check. Of course, this isn't a full rng test but it ensures that nothing terrible is happening with the implementation. It is feasible that a working rng stream wouldn't actually satisfy the uniformity requirements, since a truly random stream may never return a certain result, or at least return it very little. This is incredibly unlikely]]
add_test_cpp(openage::rng::tests::rng_tests "test functionality of rng - this *may* fail due to a statistical anomaly, but is extremely unlikely")