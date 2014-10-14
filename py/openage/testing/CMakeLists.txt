add_py_package(openage.testing)

add_test_py(openage.testing.tests.test_run_test "tests testing.run_test")
add_test_py(openage.testing.tests.test_run_demo "tests testing.run_demo")
add_test_py(openage.testing.tests.test_testspec "tests for correct python test specification file")
add_demo_py(openage.testing.tests.sample_demo "sample demo")
