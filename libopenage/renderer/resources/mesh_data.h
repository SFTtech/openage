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
};

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
	VertexInputInfo(std::vector<vertex_input_t>, vertex_layout_t);

	~VertexInputInfo() = default;

	/// Returns the size of a single vertex.
	size_t size() const;

private:
	/// What kind of data the vertices contain and how it is laid out in memory.
	std::vector<vertex_input_t> inputs;

	/// How the vertices are laid out in `data`.
	vertex_layout_t layout;
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

	~MeshData() = default;

private:
	/// The raw vertex data. The size is an integer multiple of the size of a single vertex.
	std::vector<uint8_t> data;

	/// The indices of vertices to be drawn if the mesh is indexed.
	/// For array drawing, empty optional.
	std::experimental::optional<std::vector<size_t>> ids;

	/// Information about how to interpret the data to make vertices.
	/// This is optional because initialization is deferred until the constructor
	/// obtains the mesh data e.g. from disk, but it should always be present after
	/// construction.
	std::experimental::optional<VertexInputInfo> info;
};

}}}
