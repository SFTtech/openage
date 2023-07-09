add_sources(libopenage
    buffer_info.cpp
	mesh_data.cpp
	palette_info.cpp
	shader_source.cpp
	texture_data.cpp
	texture_info.cpp
	texture_subinfo.cpp
)

add_subdirectory(animation/)
add_subdirectory(assets/)
add_subdirectory(parser/)
add_subdirectory(terrain/)
