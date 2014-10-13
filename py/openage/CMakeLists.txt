configure_file(config.py.in ${CMAKE_CURRENT_SOURCE_DIR}/config.py)

add_py_package(openage)
add_py_package(openage.testing)

add_subdirectory("convert")
