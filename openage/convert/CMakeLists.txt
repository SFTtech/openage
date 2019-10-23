add_py_modules(
	__init__.py
	binpack.py
	blendomatic.py
	changelog.py
	colortable.py
	driver.py
	drs.py
	fix_data.py
	game_versions.py
	hdlanguagefile.py
	main.py
	pefile.py
	peresource.py
	singlefile.py
	slp_converter_pool.py
	stringresource.py
	texture.py
)

add_cython_modules(
	slp.pyx
	smp.pyx
	smx.pyx
)

add_pxds(
	__init__.pxd
)

add_subdirectory(dataformat)
add_subdirectory(export)
add_subdirectory(gamedata)
add_subdirectory(hardcoded)
add_subdirectory(interface)
add_subdirectory(nyan)
add_subdirectory(opus)
add_subdirectory(processor)
