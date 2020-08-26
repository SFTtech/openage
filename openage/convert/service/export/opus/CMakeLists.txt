find_package(Ogg REQUIRED)
find_package(Opusfile REQUIRED)

add_cython_modules(
	opusenc.pyx
)

pyext_link_libraries(
	opusenc.pyx
	${OGG_LIBRARIES}
	${OPUS_LIBRARIES}
)

pyext_include_directories(
	opusenc.pyx
	${OGG_INCLUDE_DIRS}
	${OPUS_INCLUDE_DIRS}
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
