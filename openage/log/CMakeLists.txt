add_cython_module(log_cpp
	SOURCES log_cpp.pyx
)

add_py_modules(
	__init__.py
	logging.py
	tests.py
)
