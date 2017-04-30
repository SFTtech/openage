// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <array>
#include <memory>
#include <eigen3/Eigen/Core>

namespace openage {
namespace renderer {

class VertexState;
class VertexBuffer;

/// The type of geometry.
enum class geometry_t {
	quad,
	mesh,
};

/// A class representing geometry to be passed to a draw call.
// TODO this class
class Geometry {
public:
	struct Vertex {
		Eigen::Vector2f position;
		Eigen::Vector2f texture_coordinates;
	};


	/// The default constructor makes a quad.
	Geometry(VertexState* vaoPtr, VertexBuffer *vboPtr, const geometry_t &geometry_type);
	virtual ~Geometry();

	// TODO maybe use inheritance instead of enum?
	// Geometry(mesh..)

	/// Returns the type of this geometry.
	geometry_t get_type() const { return this->type; }

	virtual void draw() const = 0;

protected:
	std::unique_ptr<VertexState> vertex_state;
	std::unique_ptr<VertexBuffer> vertex_buffer;

private:
	static const std::array<Vertex, 4> quad_geometry;

	geometry_t type;

	void setup_quad();
};

}} // openage::renderer
