add_cython_module(exctranslate
	SOURCES exctranslate.pyx
)

add_cython_module(exctranslate_tests
	SOURCES exctranslate_tests.pyx
)

add_cython_module(pyobject
	SOURCES pyobject.pyx
)

add_cython_module(setup_checker
	SOURCES setup_checker.pyx
)

add_py_modules(
	__init__.py
	setup.py
)
