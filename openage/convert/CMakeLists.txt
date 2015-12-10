add_py_modules(
	__init__.py
	blendomatic.py
	changelog.py
	colortable.py
	driver.py
	drs.py
	filelist.py
	fix_data.py
	game_versions.py
	hdlanguagefile.py
	main.py
	pefile.py
	peresource.py
	slp.py
	slp_converter_pool.py
	stringresource.py
	texture.py
)

add_cython_modules(
	# hoping that this will somewhat increase the horrible performance.
	STANDALONE slp.py
)

add_subdirectory(dataformat)
add_subdirectory(gamedata)
add_subdirectory(hardcoded)
