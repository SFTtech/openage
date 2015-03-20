add_sources(${PROJECT_NAME}
	job_group.cpp
	job_manager.cpp
	tests.cpp
	worker.cpp
)

add_test_cpp(openage::job::tests::test_job_manager "Tests the job manager")
