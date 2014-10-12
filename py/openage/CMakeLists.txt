message("configuring file...")

configure_file(config.py.in ${CMAKE_CURRENT_SOURCE_DIR}/config.py)

add_py_package(openage)

add_subdirectory("convert")
