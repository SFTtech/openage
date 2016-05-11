add_py_modules(
	__init__.py
	abstract.py
	directory.py
	filecollection.py
	path.py
	union.py
	wrapper.py
)


add_cython_module(cpp
	SOURCES cpp.pyx
)
