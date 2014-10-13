add_py_package(openage.testing)

add_test_py(openage.testing.tests.test_run_test "tests the core of openage.testing, run_test")
add_test_py(openage.testing.tests.test_testspec "tests for correct python test specification file")
