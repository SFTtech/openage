// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_PIPELINE_H_
#define OPENAGE_RENDERER_PIPELINE_H_

#include <cstring>
#include <memory>
#include <type_traits>
#include <vector>

#include "program.h"
#include "vertex_buffer.h"

#include "../error/error.h"
#include "../util/compiler.h"
#include "../util/constexpr.h"
#include "../util/vector.h"

namespace openage {
namespace renderer {

class Pipeline;

/**
 * A pipeline property. Wraps GPU state to be set.
 */
class PipelineVariable {
public:
	PipelineVariable(const std::string &name,
	                 Pipeline *pipeline);
	virtual ~PipelineVariable() = default;

	/**
	 * Return the associated shader variable name.
	 */
	const std::string &get_name();

	/**
	 * Return the shader variable name as a C string.
	 */
	const char *get_name_cstr();

protected:
	/**
	 * The associated pipeline metadata container.
	 */
	Pipeline *const pipeline;

	/**
	 * Shader variable name.
	 */
	std::string name;
};


/**
 * Pipeline uniform variable,
 * which is a global value for all pipeline stages.
 */
template<typename T>
class Uniform : public PipelineVariable {
public:
	Uniform(const std::string &name, Pipeline *pipeline)
		:
		PipelineVariable{name, pipeline} {}

	virtual ~Uniform() = default;

	/**
	 * Per-type specialization of how to set the uniform value.
	 * Calls the backend-dependent function to push the data to the gpu.
	 */
	void set(const T &value);
};


/**
 * Boilerplate base class for templated vertex attributes.
 * Stores the attributes until they are packed to a uploadable buffer.
 */
class BaseAttribute : public PipelineVariable {
public:
	BaseAttribute(const std::string &name, Pipeline *pipeline)
		:
		PipelineVariable{name, pipeline} {}

	virtual ~BaseAttribute() = default;

	/**
	 * Pack all the stored attributes to the given buffer.
	 * Write a maximum of n chars.
	 */
	virtual void pack(char *buffer, size_t n) = 0;

	/**
	 * Return the number of attribute entries,
	 * aka the number of configured vertices.
	 */
	virtual size_t entry_count() = 0;

	/**
	 * Return the size in chars of one attribute entry.
	 * This equals dimension() * sizeof(attr_type)
	 */
	virtual size_t entry_size() = 0;

	/**
	 * Return the dimension of a single attribute entry.
	 * For a vec2 this is two.
	 */
	virtual size_t dimension() = 0;

	/**
	 * Return the vertex attribute type.
	 */
	virtual vertex_attribute_type type() = 0;

	/**
	 * Return the glsl layout id for this attribute.
	 */
	virtual int get_attr_id() = 0;
};


/**
 * Vertex attribute property.
 *
 * All vertex attributes of a shader have the same number of entries!
 * All of those attribtes are merged into a single buffer on request.
 * The buffer merging is done in the respective context.
 * This buffer is then transmitted to the GPU.
 *
 * You may only use types for T that can be copied by memcpy to a
 * char buffer.
 *
 * We could extend this to support any class by specializing
 * the pack method for POD types and some other magic base class type
 * that provides the availability of a specific packing method.
 */
template<typename T,
         vertex_attribute_type attribute_type,
         size_t dimensions>
class Attribute : public BaseAttribute {
public:
	Attribute(const std::string &name, Pipeline *pipeline)
		:
		BaseAttribute{name, pipeline},
		attr_id{-1} {}

	virtual ~Attribute() = default;

	// as we wanna copy the values to the gpu, they need to be
	// easily copyable.
	static_assert(std::is_pod<T>::value,
	              "only plain old datatypes supported as attributes");

	/**
	 * Set this attribute to some value array.
	 */
	void set(const std::vector<T> &values) {
		this->values = values;
	}

	/**
	 * Set the glsl layout position.
	 * if unset, determine the position automatically.
	 */
	void set_layout(unsigned int position) {
		this->attr_id = position;
	}

	/**
	 * return the glsl layout position.
	 * If it's unknown until now, determine it by querying the gpu.
	 */
	int get_attr_id() override {
		if (unlikely(this->attr_id < 0)) {
			this->attr_id = this->pipeline->program->get_attribute_id(this->get_name_cstr());
		}

		return this->attr_id;
	}

	/**
	 * Store the data to the given buffer.
	 * This could be extended to also support other than POD types.
	 */
	void pack(char *buffer, size_t n) override {
		memcpy(buffer, &this->values[0], n);
	}

	/**
	 * Return the number of configured vertices.
	 */
	size_t entry_count() override {
		return this->values.size();
	}

	/**
	 * Compute the size of a full vertex attribute data value.
	 * Equals dimensions * sizeof(entry_type)
	 */
	size_t entry_size() override {
		return this->dimension() * util::compiletime<size_t, vertex_attribute_size[attribute_type]>();
	}

	/**
	 * Return the dimension of one vertex configuration entry.
	 */
	size_t dimension() override {
		return dimensions;
	}

	/**
	 * Provide the type of one component of one vertex attribute entry.
	 */
	vertex_attribute_type type() override {
		return attribute_type;
	}

protected:
	/**
	 * The vertex attribute values to be packed and submitted.
	 * We need to cache the values as the buffer packing and transfer
	 * is done after multiple attributes were set.
	 */
	std::vector<T> values;

	/**
	 * glsl layout position of this attribute.
	 */
	int attr_id;
};


/**
 * Inherit from this class to create statically known pipeline properties.
 *
 * Add members of PipelineVariable to the inherited class
 * to make pipeline variables available to the outside.
 * This buffer is then transmitted to the GPU when the time has come.
 */
class Pipeline {
public:
	Pipeline(Program *prg);
	virtual ~Pipeline() = default;

	/**
	 * Add the given program variable to the list of maintained
	 * pipeline attributes.
	 */
	void add_var(PipelineVariable *var);

	/**
	 * Create a vertex buffer that stores the attribute
	 * values set in this pipeline.
	 */
	VertexBuffer create_attribute_buffer();

	/**
	 * Update the given vertex buffer with attributes set in this
	 * pipeline instance.
	 */
	void update_buffer(VertexBuffer *vbuf);

	/**
	 * The pipeline program associated with this property definition class.
	 */
	Program *const program;

protected:

	/**
	 * Syncs attribute entry lengths.
	 * Each attribute has to be supplied for each vertex once.
	 * e.g. vec3 1337 color entries require 1337 vec4 positions.
	 * These have different per-attribute sizes but the same lengths.
	 */
	std::vector<BaseAttribute *> attributes;
};

}} // openage::renderer

#endif
