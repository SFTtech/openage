# python module configurations

# python config file is created in libopenage
# in order to get options from libopenage

add_py_modules(
	__init__.py
	__main__.py
	assets.py
	${CMAKE_CURRENT_BINARY_DIR}/config.py
	default_dirs.py
	NOINSTALL devmode.py
)

add_cython_modules(
	cython_check.pyx
)

add_subdirectory(cabextract)
add_subdirectory(codegen)
add_subdirectory(convert)
add_subdirectory(cppinterface)
add_subdirectory(cvar)
add_subdirectory(event)
add_subdirectory(game)
add_subdirectory(log)
add_subdirectory(nyan)
add_subdirectory(util)
add_subdirectory(renderer)
add_subdirectory(testing)
add_subdirectory(versions)
