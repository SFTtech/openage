# convert script deploying

add_py_package(openage.convert)
add_py_package(openage.convert.gamedata)
add_py_package(openage.convert.hardcoded)
add_py_package(openage.convert.cabextract)

add_pyext_module(
	openage.convert.cabextract.lzxd

	cabextract/lzxd/lzxd.cpp
	cabextract/lzxd/util.cpp
	cabextract/lzxd/pyinterface.cpp
)
