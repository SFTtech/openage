add_cython_modules(
	lzxd.pyx
	cabchecksum.pyx
)

add_py_modules(
	__init__.py
	cab.py
	lzxdstream.py
	test.py
)
