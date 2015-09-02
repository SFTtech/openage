# create python config file
get_config_option_string()
configure_file(config.py.in "${CMAKE_SOURCE_DIR}/openage/config.py")

set_source_files_properties(devmode.py PROPERTIES NOINSTALL TRUE)

add_py_modules(
	__init__.py
	__main__.py
	assets.py
	config.py
	devmode.py
)

add_subdirectory(cabextract)
add_subdirectory(codegen)
add_subdirectory(convert)
add_subdirectory(cppinterface)
add_subdirectory(game)
add_subdirectory(log)
add_subdirectory(nyan)
add_subdirectory(util)
add_subdirectory(testing)
