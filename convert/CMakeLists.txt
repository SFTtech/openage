# convert script deploying

add_py_module(openage.convert)
add_py_module(openage.convert.gamedata)
add_py_module(openage.convert.hardcoded)
add_py_module(openage.convert.cabextract)

add_pyext_module(
	NAME openage.convert.cabextract.lzxd
	SOURCES
	cabextract/lzxd/lzxd.cpp
	cabextract/lzxd/util.cpp
	cabextract/lzxd/pyinterface.cpp
)
