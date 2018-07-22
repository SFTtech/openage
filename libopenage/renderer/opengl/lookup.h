// Copyright 2018-2018 the openage authors. See copying.md for legal info.

// Lookup tables for translating between OpenGL-specific values and generic renderer values,
// as well as mapping things like type sizes within OpenGL.

#pragma once

#include <epoxy/gl.h>

#include "../resources/texture_info.h"
#include "../resources/mesh_data.h"


namespace openage {
namespace renderer {
namespace opengl {

/// Input and output pixel formats from pixel_format.
static constexpr auto GL_PIXEL_FORMAT = datastructure::create_const_map<resources::pixel_format, std::tuple<GLint, GLenum, GLenum>>(
	// TODO check correctness of formats here
	std::make_pair(resources::pixel_format::r16ui, std::make_tuple(GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_INT)),
	std::make_pair(resources::pixel_format::r32ui, std::make_tuple(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT)),
	std::make_pair(resources::pixel_format::rgb8, std::make_tuple(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::bgr8, std::make_tuple(GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::rgba8, std::make_tuple(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::rgba8ui, std::make_tuple(GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::depth24, std::make_tuple(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE))
);

/// Sizes of various uniform/vertex input types in shaders.
static constexpr auto GL_SHADER_TYPE_SIZE = datastructure::create_const_map<GLenum, size_t>(
	std::make_pair(GL_FLOAT, 4),
	std::make_pair(GL_FLOAT_VEC2, 8),
	std::make_pair(GL_FLOAT_VEC3, 12),
	std::make_pair(GL_FLOAT_VEC4, 16),
	std::make_pair(GL_INT, 4),
	std::make_pair(GL_INT_VEC2, 8),
	std::make_pair(GL_INT_VEC3, 12),
	std::make_pair(GL_INT_VEC4, 16),
	std::make_pair(GL_UNSIGNED_INT, 4),
	std::make_pair(GL_UNSIGNED_INT_VEC2, 8),
	std::make_pair(GL_UNSIGNED_INT_VEC3, 12),
	std::make_pair(GL_UNSIGNED_INT_VEC4, 16),
	std::make_pair(GL_BOOL, 1),
	std::make_pair(GL_BOOL_VEC2, 2),
	std::make_pair(GL_BOOL_VEC3, 3),
	std::make_pair(GL_BOOL_VEC4, 4),
	std::make_pair(GL_FLOAT_MAT2, 16),
	std::make_pair(GL_FLOAT_MAT3, 36),
	std::make_pair(GL_FLOAT_MAT4, 64),
	std::make_pair(GL_SAMPLER_1D, 4),
	std::make_pair(GL_SAMPLER_2D, 4),
	std::make_pair(GL_SAMPLER_2D_ARRAY, 4),
	std::make_pair(GL_SAMPLER_3D, 4),
	std::make_pair(GL_SAMPLER_CUBE, 4)
);

/// GLSL type strings with corresponding GL types.
static constexpr auto GLSL_TYPE_NAME = datastructure::create_const_map<const char*, GLenum>(
	std::make_pair("int", GL_INT),
	std::make_pair("uint", GL_UNSIGNED_INT),
	std::make_pair("float", GL_FLOAT),
	std::make_pair("double", GL_DOUBLE),
	std::make_pair("vec2", GL_FLOAT_VEC2),
	std::make_pair("vec3", GL_FLOAT_VEC3),
	std::make_pair("mat3", GL_FLOAT_MAT3),
	std::make_pair("mat4", GL_FLOAT_MAT4),
	std::make_pair("ivec2", GL_INT_VEC2),
	std::make_pair("ivec3", GL_INT_VEC3),
	std::make_pair("sampler2D", GL_SAMPLER_2D),
	std::make_pair("sampler2DArray", GL_SAMPLER_2D_ARRAY)
);

/// Generic vertex input types from GL types.
static constexpr auto GL_VERT_IN_TYPE = datastructure::create_const_map<GLenum, resources::vertex_input_t>(
	std::make_pair(GL_FLOAT, resources::vertex_input_t::F32),
	std::make_pair(GL_FLOAT_VEC2, resources::vertex_input_t::V2F32),
	std::make_pair(GL_FLOAT_VEC3, resources::vertex_input_t::V3F32),
	std::make_pair(GL_FLOAT_MAT3, resources::vertex_input_t::M3F32)
);

/// The type of a single element in a per-vertex attribute.
static constexpr auto GL_VERT_IN_ELEM_TYPE = datastructure::create_const_map<resources::vertex_input_t, GLenum>(
	std::make_pair(resources::vertex_input_t::F32, GL_FLOAT),
	std::make_pair(resources::vertex_input_t::V2F32, GL_FLOAT),
	std::make_pair(resources::vertex_input_t::V3F32, GL_FLOAT),
	std::make_pair(resources::vertex_input_t::M3F32, GL_FLOAT)
);

/// Mapping from generic primitive types to GL types.
static constexpr auto GL_PRIMITIVE = datastructure::create_const_map<resources::vertex_primitive_t, GLenum>(
	std::make_pair(resources::vertex_primitive_t::POINTS, GL_POINTS),
  std::make_pair(resources::vertex_primitive_t::LINES, GL_LINES),
  std::make_pair(resources::vertex_primitive_t::LINE_STRIP, GL_LINE_STRIP),
  std::make_pair(resources::vertex_primitive_t::TRIANGLES, GL_TRIANGLES),
  std::make_pair(resources::vertex_primitive_t::TRIANGLE_STRIP, GL_TRIANGLE_STRIP),
  std::make_pair(resources::vertex_primitive_t::TRIANGLE_FAN, GL_TRIANGLE_FAN)
);

/// Mapping from generic index types to GL types.
static constexpr auto GL_INDEX_TYPE = datastructure::create_const_map<resources::index_t, GLenum>(
	std::make_pair(resources::index_t::U8, GL_UNSIGNED_BYTE),
	std::make_pair(resources::index_t::U16, GL_UNSIGNED_SHORT),
	std::make_pair(resources::index_t::U32, GL_UNSIGNED_INT)
);

}}} // openage::renderer::opengl
