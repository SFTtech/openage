add_py_modules(
	__init__.py
	blendomatic.py
	colortable.py
	driver.py
	drs.py
	filelist.py
	fix_data.py
	hdlanguagefile.py
	main.py
	pefile.py
	peresource.py
	slp.py
	stringresource.py
	texture.py
)

add_cython_modules(
	# hoping that this will somewhat increase the horrible performance.
	slp.py
)

add_subdirectory(dataformat)
add_subdirectory(gamedata)
add_subdirectory(hardcoded)
