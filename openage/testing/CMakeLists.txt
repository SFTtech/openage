add_cython_module(cpp_testing
	SOURCES cpp_testing.pyx
)

add_cython_module(misc_cpp
	SOURCES misc_cpp.pyx
)

add_py_modules(
	__init__.py
	doctest.py
	list_processor.py
	main.py
	testing.py
	testlist.py
)
