add_py_modules(
	__init__.py
	abstract.py
	directory.py
	filecollection.py
	path.py
	test.py
	union.py
	wrapper.py
)

add_pxds(
	cpp.pxd
)

add_cython_modules(
	cpp.pyx
)
