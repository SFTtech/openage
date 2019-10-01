// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>
#include <optional>

#include "../../util/path.h"


namespace openage {
namespace renderer {
namespace resources {

/// The type of a single per-vertex input to the shader program.
enum class vertex_input_t {
	F32,
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

	/// Adds a mesh->shader input mapping to the info. By default, attributes are mapped
	/// one-to-one according to their order in the input vector, e.g. (vec2 pos, vec2 uv)
	/// maps into (0: vec2 pos, 1: vec2 uv) in the shader. However, if a shader skips indices
	/// in the layout or takes its inputs in a different order, this can be specified using the
	/// map. The map entries must have the format (index_in_vector, index_in_shader) and will
	/// overwrite the default mapping. If an entry for a given index in the vector is missing,
	/// that attribute and its data will be skipped.
	void add_shader_input_map(std::unordered_map<size_t, size_t>&&);

	/// Returns the list of per-vertex inputs.
	const std::vector<vertex_input_t> &get_inputs() const;

	/// Returns the shader input map or an empty optional if it's not present.
	std::optional<std::unordered_map<size_t, size_t>> const& get_shader_input_map() const;

	/// Returns the layout of vertices in memory.
	vertex_layout_t get_layout() const;

	/// Returns the size of a single vertex.
	size_t vert_size() const;

	/// Returns the primitive interpretation mode.
	vertex_primitive_t get_primitive() const;

	/// Returns the type of indices used for indexed drawing,
	/// which may not be present if array drawing is used.
	std::optional<index_t> get_index_type() const;

private:
	/// What kind of data the vertices contain and how it is laid out in memory.
	std::vector<vertex_input_t> inputs;

	/// An optional attribute specifying how vertex inputs in the mesh map to vertex inputs
	/// in a given shader, for example to reorder inputs of the form (pos, uv) into a shader
	/// that takes in (uv, pos) inputs.
	std::optional<std::unordered_map<size_t, size_t>> shader_input_map;

	/// How the vertices are laid out in the data buffer.
	vertex_layout_t layout;

	/// The primitive type.
	vertex_primitive_t primitive;

	/// The type of indices if they exist.
	std::optional<index_t> index_type;
};

class MeshData {
public:
	/// Tries to load the mesh data from the specified file.
	explicit MeshData(const util::Path&);

	/// Initializes the mesh data to a custom unindexed vertex vector described by the given info.
	MeshData(std::vector<uint8_t> &&verts, VertexInputInfo);

	/// Initializes the mesh data to a custom indexed vertex vector described by the given info.
	MeshData(std::vector<uint8_t> &&verts, std::vector<uint8_t> &&ids, VertexInputInfo);

	/// Returns the raw vertex data.
	std::vector<uint8_t> const &get_data() const;

	/// Returns the indices used for indexed drawing if they exist.
	std::optional<std::vector<uint8_t>> const &get_ids() const;

	/// Returns information describing the vertices in this mesh.
	VertexInputInfo get_info() const;

	/// Initializes the mesh data with a simple quadrilateral spanning the whole window space,
	/// with (vec2 pos, vec2 uv) vertex format.
	static MeshData make_quad();

private:
	/// The raw vertex data. The size is an integer multiple of the size of a single vertex.
	std::vector<uint8_t> data;

	/// The indices of vertices to be drawn if the mesh is indexed.
	/// For array drawing, empty optional.
	std::optional<std::vector<uint8_t>> ids;

	/// Information about how to interpret the data to make vertices.
	/// This is optional because initialization is deferred until the constructor
	/// obtains the mesh data e.g. from disk, but it should always be present after
	/// construction.
	std::optional<VertexInputInfo> info;
};

}}}
