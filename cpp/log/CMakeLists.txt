add_test_cpp(openage::log::tests::test "tests the logging")

add_sources(${PROJECT_NAME}
	file_logsink.cpp
	level.cpp
	log.cpp
	logsource.cpp
	logsink.cpp
	message.cpp
	named_logsource.cpp
	stdout_logsink.cpp
	test.cpp
)
