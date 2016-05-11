add_cython_module(lzxd
	SOURCES lzxd.pyx
)

add_cython_module(cabchecksum
	STANDALONE
	SOURCES cabchecksum.pyx
)

add_py_modules(
	__init__.py
	cab.py
	lzxdstream.py
	test.py
)
