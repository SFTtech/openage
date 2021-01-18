add_py_modules(
	__init__.py
	data_exporter.py
	generate_manifest_hashes.py
	media_exporter.py
	modpack_exporter.py
)

add_cython_modules(
	terrain_merge.pyx
	texture_merge.pyx
)
