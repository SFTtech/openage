add_py_modules(
	__init__.py
	blendomatic.py
	colortable.py
	drs.py
)

add_cython_modules(
	slp.pyx
	smp.pyx
	smx.pyx
)

add_pxds(
	__init__.pxd
)