add_cython_modules(
	opusenc.pyx
)

add_pxds(
	__init__.pxd
	bytearray.pxd
	ogg.pxd
	opus.pxd
)

add_py_modules(
	__init__.py
	demo.py
)
