add_py_package(openage)

add_test_py(openage.assets.test "tests python asset locator")

add_subdirectory("convert")
add_subdirectory("testing")
