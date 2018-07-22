# python module configurations

# create python config file
get_config_option_string()
configure_file(config.py.in "${CMAKE_SOURCE_DIR}/openage/config.py")

add_py_modules(
	__init__.py
	__main__.py
	assets.py
	config.py
	default_dirs.py
	NOINSTALL devmode.py
)

add_subdirectory(cabextract)
add_subdirectory(codegen)
add_subdirectory(convert)
add_subdirectory(cppinterface)
add_subdirectory(cvar)
add_subdirectory(game)
add_subdirectory(log)
add_subdirectory(util)
add_subdirectory(renderer)
add_subdirectory(testing)
