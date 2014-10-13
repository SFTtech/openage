add_py_package(openage)

add_test_py(openage.assets.test "tests global asset locator")

add_subdirectory("convert")
add_subdirectory("testing")
