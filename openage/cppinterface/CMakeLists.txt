add_cython_modules(
	exctranslate.pyx
	exctranslate_tests.pyx
	pyobject.pyx
	setup_checker.pyx
)

add_pxds(
	typedefs.pxd
)

add_py_modules(
	__init__.py
	setup.py
)
