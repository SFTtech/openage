# create python config file
get_config_option_string()
configure_file(config.py.in "${CMAKE_SOURCE_DIR}/openage/config.py")

add_py_packages(
	openage
	openage.cabextract
	openage.convert
	openage.convert.gamedata
	openage.convert.hardcoded
	openage.cppinterface
	openage.game
	openage.log
	openage.testing
	openage.util
)

add_cython_modules(
	cabextract/lzxd.pyx
	cabextract/cabchecksum.pyx
	cppinterface/exctranslate.pyx
	cppinterface/exctranslate_tests.pyx
	cppinterface/pyobject.pyx
	cppinterface/setup_checker.pyx
	game/main_cpp.pyx
	log/log_cpp.pyx
	util/fslike_cpp.pyx
	testing/cpp_testing.pyx
	testing/misc_cpp.pyx
)
