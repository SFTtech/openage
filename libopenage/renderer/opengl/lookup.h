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
	std::pair(resources::pixel_format::r16ui, std::tuple(GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_INT)),
	std::pair(resources::pixel_format::r32ui, std::tuple(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT)),
	std::pair(resources::pixel_format::rgb8, std::tuple(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE)),
	std::pair(resources::pixel_format::bgr8, std::tuple(GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE)),
	std::pair(resources::pixel_format::rgba8, std::tuple(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE)),
	std::pair(resources::pixel_format::rgba8ui, std::tuple(GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE)),
	std::pair(resources::pixel_format::depth24, std::tuple(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE))
);

/// Sizes of various uniform/vertex input types in shaders.
static constexpr auto GL_SHADER_TYPE_SIZE = datastructure::create_const_map<GLenum, size_t>(
	std::pair(GL_FLOAT, 4),
	std::pair(GL_FLOAT_VEC2, 8),
	std::pair(GL_FLOAT_VEC3, 12),
	std::pair(GL_FLOAT_VEC4, 16),
	std::pair(GL_INT, 4),
	std::pair(GL_INT_VEC2, 8),
	std::pair(GL_INT_VEC3, 12),
	std::pair(GL_INT_VEC4, 16),
	std::pair(GL_UNSIGNED_INT, 4),
	std::pair(GL_UNSIGNED_INT_VEC2, 8),
	std::pair(GL_UNSIGNED_INT_VEC3, 12),
	std::pair(GL_UNSIGNED_INT_VEC4, 16),
	std::pair(GL_BOOL, 1),
	std::pair(GL_BOOL_VEC2, 2),
	std::pair(GL_BOOL_VEC3, 3),
	std::pair(GL_BOOL_VEC4, 4),
	std::pair(GL_FLOAT_MAT2, 16),
	std::pair(GL_FLOAT_MAT3, 36),
	std::pair(GL_FLOAT_MAT4, 64),
	std::pair(GL_SAMPLER_1D, 4),
	std::pair(GL_SAMPLER_2D, 4),
	std::pair(GL_SAMPLER_2D_ARRAY, 4),
	std::pair(GL_SAMPLER_3D, 4),
	std::pair(GL_SAMPLER_CUBE, 4)
);

/// GL types with corresponding GLSL type strings.
static constexpr auto GLSL_TYPE_NAME = datastructure::create_const_map<GLenum, const char*>(
	std::pair(GL_FLOAT, "float"),
	std::pair(GL_FLOAT_VEC2, "vec2"),
	std::pair(GL_FLOAT_VEC3, "vec3"),
	std::pair(GL_FLOAT_VEC4, "vec4"),
	std::pair(GL_INT, "int"),
	std::pair(GL_INT_VEC2, "ivec2"),
	std::pair(GL_INT_VEC3, "ivec3"),
	std::pair(GL_INT_VEC4, "ivec4"),
	std::pair(GL_UNSIGNED_INT, "uint"),
	std::pair(GL_UNSIGNED_INT_VEC2, "uvec2"),
	std::pair(GL_UNSIGNED_INT_VEC3, "uvec3"),
	std::pair(GL_UNSIGNED_INT_VEC4, "uvec4"),
	std::pair(GL_BOOL, "bool"),
	std::pair(GL_BOOL_VEC2, "bvec2"),
	std::pair(GL_BOOL_VEC3, "bvec3"),
	std::pair(GL_BOOL_VEC4, "bvec4"),
	std::pair(GL_FLOAT_MAT2, "mat2"),
	std::pair(GL_FLOAT_MAT3, "mat3"),
	std::pair(GL_FLOAT_MAT4, "mat4"),
	std::pair(GL_FLOAT_MAT2x3, "mat2x3"),
	std::pair(GL_FLOAT_MAT2x4, "mat2x4"),
	std::pair(GL_FLOAT_MAT3x2, "mat3x2"),
	std::pair(GL_FLOAT_MAT3x4, "mat3x4"),
	std::pair(GL_FLOAT_MAT4x2, "mat4x2"),
	std::pair(GL_FLOAT_MAT4x3, "mat4x3"),
	std::pair(GL_SAMPLER_1D, "sampler1D"),
	std::pair(GL_SAMPLER_2D, "sampler2D"),
	std::pair(GL_SAMPLER_3D, "sampler3D"),
	std::pair(GL_SAMPLER_CUBE, "samplerCube"),
	std::pair(GL_SAMPLER_1D_SHADOW, "sampler1DShadow"),
	std::pair(GL_SAMPLER_2D_SHADOW, "sampler2DShadow"),
	std::pair(GL_SAMPLER_1D_ARRAY, "sampler1DArray"),
	std::pair(GL_SAMPLER_2D_ARRAY, "sampler2DArray"),
	std::pair(GL_SAMPLER_1D_ARRAY_SHADOW, "sampler1DArrayShadow"),
	std::pair(GL_SAMPLER_2D_ARRAY_SHADOW, "sampler2DArrayShadow"),
	std::pair(GL_SAMPLER_2D_MULTISAMPLE, "sampler2DMS"),
	std::pair(GL_SAMPLER_2D_MULTISAMPLE_ARRAY, "sampler2DMSArray"),
	std::pair(GL_SAMPLER_CUBE_SHADOW, "samplerCubeShadow"),
	std::pair(GL_SAMPLER_BUFFER, "samplerBuffer"),
	std::pair(GL_SAMPLER_2D_RECT, "sampler2DRect"),
	std::pair(GL_SAMPLER_2D_RECT_SHADOW, "sampler2DRectShadow"),
	std::pair(GL_INT_SAMPLER_1D, "isampler1D"),
	std::pair(GL_INT_SAMPLER_2D, "isampler2D"),
	std::pair(GL_INT_SAMPLER_3D, "isampler3D"),
	std::pair(GL_INT_SAMPLER_CUBE, "isamplerCube"),
	std::pair(GL_INT_SAMPLER_1D_ARRAY, "isampler1DArray"),
	std::pair(GL_INT_SAMPLER_2D_ARRAY, "isampler2DArray"),
	std::pair(GL_INT_SAMPLER_2D_MULTISAMPLE, "isampler2DMS"),
	std::pair(GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, "isampler2DMSArray"),
	std::pair(GL_INT_SAMPLER_BUFFER, "isamplerBuffer"),
	std::pair(GL_INT_SAMPLER_2D_RECT, "isampler2DRect"),
	std::pair(GL_UNSIGNED_INT_SAMPLER_1D, "usampler1D"),
	std::pair(GL_UNSIGNED_INT_SAMPLER_2D, "usampler2D"),
	std::pair(GL_UNSIGNED_INT_SAMPLER_3D, "usampler3D"),
	std::pair(GL_UNSIGNED_INT_SAMPLER_CUBE, "usamplerCube"),
	std::pair(GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, "usampler2DArray"),
	std::pair(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, "usampler2DArray"),
	std::pair(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, "usampler2DMS"),
	std::pair(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, "usampler2DMSArray"),
	std::pair(GL_UNSIGNED_INT_SAMPLER_BUFFER, "usamplerBuffer"),
	std::pair(GL_UNSIGNED_INT_SAMPLER_2D_RECT, "usampler2DRect")
);

/// Generic vertex input types from GL types.
static constexpr auto GL_VERT_IN_TYPE = datastructure::create_const_map<GLenum, resources::vertex_input_t>(
	std::pair(GL_FLOAT, resources::vertex_input_t::F32),
	std::pair(GL_FLOAT_VEC2, resources::vertex_input_t::V2F32),
	std::pair(GL_FLOAT_VEC3, resources::vertex_input_t::V3F32),
	std::pair(GL_FLOAT_MAT3, resources::vertex_input_t::M3F32)
);

/// The type of a single element in a per-vertex attribute.
static constexpr auto GL_VERT_IN_ELEM_TYPE = datastructure::create_const_map<resources::vertex_input_t, GLenum>(
	std::pair(resources::vertex_input_t::F32, GL_FLOAT),
	std::pair(resources::vertex_input_t::V2F32, GL_FLOAT),
	std::pair(resources::vertex_input_t::V3F32, GL_FLOAT),
	std::pair(resources::vertex_input_t::M3F32, GL_FLOAT)
);

/// Mapping from generic primitive types to GL types.
static constexpr auto GL_PRIMITIVE = datastructure::create_const_map<resources::vertex_primitive_t, GLenum>(
	std::pair(resources::vertex_primitive_t::POINTS, GL_POINTS),
	std::pair(resources::vertex_primitive_t::LINES, GL_LINES),
	std::pair(resources::vertex_primitive_t::LINE_STRIP, GL_LINE_STRIP),
	std::pair(resources::vertex_primitive_t::TRIANGLES, GL_TRIANGLES),
	std::pair(resources::vertex_primitive_t::TRIANGLE_STRIP, GL_TRIANGLE_STRIP),
	std::pair(resources::vertex_primitive_t::TRIANGLE_FAN, GL_TRIANGLE_FAN)
);

/// Mapping from generic index types to GL types.
static constexpr auto GL_INDEX_TYPE = datastructure::create_const_map<resources::index_t, GLenum>(
	std::pair(resources::index_t::U8, GL_UNSIGNED_BYTE),
	std::pair(resources::index_t::U16, GL_UNSIGNED_SHORT),
	std::pair(resources::index_t::U32, GL_UNSIGNED_INT)
);

}}} // openage::renderer::opengl
