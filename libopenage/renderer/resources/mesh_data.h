// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <cstdint>
#include <cstdlib>
#include <experimental/optional>

#include "../../util/path.h"


namespace openage {
namespace renderer {
namespace resources {

/// The type of a single per-vertex input to the shader program.
enum class vertex_input_t {
	V2F32,
	V3F32,
	M3F32,
};

/// The primitive type that the vertices in a mesh combine into.
enum class vertex_primitive_t {
	POINTS,
	LINES,
	LINE_STRIP,
	TRIANGLES,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
};

/// The type of indices used for indexed rendering.
enum class index_t {
	U8,
	U16,
	U32,
};

/// Returns the size in bytes of a per-vertex input.
size_t vertex_input_size(vertex_input_t);

/// Returns the number of elements of the underlying type in a given per-vertex input type.
/// For example, V3F32 has 3 float elements, so the value is 3.
size_t vertex_input_count(vertex_input_t);

enum class vertex_layout_t {
	/// Array of structs. XYZUV, XYZUV, XYZUV
	AOS,
	/// Struct of arrays. XYZXYZXYZ, UVUVUV
	SOA,
};

/// Information about vertex input data - which components a vertex contains
/// and how vertices are laid out in memory.
class VertexInputInfo {
public:
	/// Initialize an input info for array rendering.
	VertexInputInfo(std::vector<vertex_input_t>, vertex_layout_t, vertex_primitive_t);

	/// Initialize an input info for indexed rendering.
	VertexInputInfo(std::vector<vertex_input_t>, vertex_layout_t, vertex_primitive_t, index_t);

	/// Returns the list of per-vertex inputs.
	const std::vector<vertex_input_t> &get_inputs() const;

	/// Returns the layout of vertices in memory.
	vertex_layout_t get_layout() const;

	/// Returns the size of a single vertex.
	size_t vert_size() const;

	/// Returns the primitive interpretation mode.
	vertex_primitive_t get_primitive() const;

	/// Returns the type of indices used for indexed drawing,
	/// which may not be present if array drawing is used.
	std::experimental::optional<index_t> get_index_type() const;

private:
	/// What kind of data the vertices contain and how it is laid out in memory.
	std::vector<vertex_input_t> inputs;

	/// How the vertices are laid out in the data buffer.
	vertex_layout_t layout;

	/// The primitive type.
	vertex_primitive_t primitive;

	/// The type of indices if they exist.
	std::experimental::optional<index_t> index_type;
};

/// An empty struct used to initialize mesh data to a quad.
struct init_quad_t {};

class MeshData {
public:
	/// Tries to load the mesh data from the specified file.
	explicit MeshData(const util::Path&);

	/// Initializes the mesh data to a quad.
	// TODO the empty struct is ugly
	explicit MeshData(init_quad_t);

	/// Returns the raw vertex data.
	std::vector<uint8_t> const &get_data() const;

	/// Returns the indices used for indexed drawing if they exist.
	std::experimental::optional<std::vector<uint8_t>> const &get_ids() const;

	/// Returns information describing the vertices in this mesh.
	VertexInputInfo get_info() const;

private:
	/// The raw vertex data. The size is an integer multiple of the size of a single vertex.
	std::vector<uint8_t> data;

	/// The indices of vertices to be drawn if the mesh is indexed.
	/// For array drawing, empty optional.
	std::experimental::optional<std::vector<uint8_t>> ids;

	/// Information about how to interpret the data to make vertices.
	/// This is optional because initialization is deferred until the constructor
	/// obtains the mesh data e.g. from disk, but it should always be present after
	/// construction.
	std::experimental::optional<VertexInputInfo> info;
};

}}}
