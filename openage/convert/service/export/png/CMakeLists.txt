find_package(PNG REQUIRED)

add_cython_modules(
	png_create.pyx
)

pyext_link_libraries(
	png_create.pyx
	PNG::PNG
)

add_pxds(
	__init__.pxd
	libpng.pxd
)

add_py_modules(
	__init__.py
)
