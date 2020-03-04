find_package(Libpng REQUIRED)

# Currently there is no way to link cython modules to extra libraries.
# Since PYEXT_LINK_LIBRARY normally only includes libopenage (what
# opusenc doesn't need), we hijack this variable. This is ok, because
# there are no subdirectories, that will see the changed variable.
set(PYEXT_LINK_LIBRARY
	${LIBPNG_LIBRARIES}
)

set(PYEXT_INCLUDE_DIRS
	${PYEXT_INCLUDE_DIRS}
	${LIBPNG_INCLUDE_DIRS}
)

add_cython_modules(
	png_create.pyx
)

add_pxds(
	__init__.pxd
	libpng.pxd
)

add_py_modules(
	__init__.py
)
