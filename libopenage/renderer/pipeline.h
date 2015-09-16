// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_PIPELINE_H_
#define OPENAGE_RENDERER_PIPELINE_H_

#include <memory>

#include "program.h"

#include "../error/error.h"
#include "../util/compiler.h"

namespace openage {
namespace renderer {

/**
 * A pipeline property. Wraps GPU state to be set.
 */
class PipelineVariable {
public:
	PipelineVariable(Program *program);
	virtual ~PipelineVariable();

	/**
	 * Assign this uniform to the given pipeline program.
	 */
	void set_program(Program *program);

	/**
	 * Set the shader-defined uniform variable name.
	 */
	void set_name(const std::string &name);

protected:
	/**
	 * The associated gpu program.
	 */
	Program *program;

	/**
	 * Shader variable name.
	 */
	std::string name;
};


/**
 * Pipeline uniform variable, which is a global value for all shader stages.
 */
template<typename T>
class Uniform : public PipelineVariable {
public:
	Uniform(Program *program=nullptr)
		:
		PipelineVariable{program} {
	}

	virtual ~Uniform() {};

	/**
	 * set the uniform value to the gpu.
	 */
	void set(const T &value) {
		if (unlikely(this->program == nullptr)) {
			throw error::Error(MSG(err) << "can't set uniform when its program is unset.");
		}
		this->set_impl(value);
	}

	/**
	 * Per-type specialization of how to set the uniform value.
	 */
	void set_impl(const T &value);
};


/**
 * Vertex attribute property.
 *
 * All vertex attributes of a shader have the same number of entries!
 * All of those attribtes are merged into a single buffer on request.
 * The buffer merging is done in the respective context.
 * This buffer is then transmitted to the GPU.
 */
template<class T>
class Attribute : public PipelineVariable {
public:
	Attribute(Program *program=nullptr)
		:
		PipelineVariable{program} {
	}
	virtual ~Attribute() {};

protected:
	/**
	 * The vertex attribute values
	 */
	std::vector<T> values;
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
	virtual ~Pipeline();

protected:
	/**
	 * Add the given program variable to the list of maintained
	 * pipeline attributes.
	 */
	void add_var(const std::string &name, PipelineVariable &var);

	/**
	 * The pipeline program associated with this property definition class.
	 */
	Program *program;

	/**
	 * Syncs attribute entry lengths.
	 * Each attribute has to be supplied for each vertex once.
	 * e.g. vec3 1337 color entries require 1337 vec4 positions.
	 * These have different per-attribute sizes but the same lengths.
	 */
	std::unordered_map<std::string, PipelineVariable *> variables;
};

}} // openage::renderer

#endif
