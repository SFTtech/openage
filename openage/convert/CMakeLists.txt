add_py_modules(
	__init__.py
	binpack.py
	changelog.py
	main.py
	slp_converter_pool.py
	texture.py
)

add_subdirectory(deprecated)
add_subdirectory(entity_object)
add_subdirectory(processor)
add_subdirectory(service)
add_subdirectory(tool)
add_subdirectory(value_object)
