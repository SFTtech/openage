add_sources(libopenage
	compiler.cpp
	constinit_vector.cpp
	enum.cpp
	enum_test.cpp
	externalprofiler.cpp
	externalsstream.cpp
	file.cpp
	fds.cpp
	fixed_point.cpp
	fixed_point_test.cpp
	fps.cpp
	hash.cpp
	hash_test.cpp
	init.cpp
	language.cpp
	matrix.cpp
	matrix_test.cpp
	misc.cpp
	misc_test.cpp
	os.cpp
	path.cpp
	quaternion.cpp
	quaternion_test.cpp
	repr.cpp
	stringformatter.cpp
	strings.cpp
	subprocess.cpp
	thread_id.cpp
	timer.cpp
	timing.cpp
	unicode.cpp
	vector.cpp
	vector_test.cpp
)

# C++ module unit
add_sources(libopenage MODULE
	color.cpp
)

pxdgen(
	compiler.h
	enum.h
	enum_test.h
	file.h
	path.h
)

add_subdirectory(compress)
add_subdirectory(filelike)
add_subdirectory(fslike)
