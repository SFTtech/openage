configure_file(config.py.in ${CMAKE_CURRENT_SOURCE_DIR}/config.py)

add_py_package(openage)

add_test_py(openage.assets.test "tests global asset locator")

add_subdirectory("convert")
add_subdirectory("testing")
