// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_PIPELINE_H_
#define OPENAGE_RENDERER_PIPELINE_H_


#include "program.h"


namespace openage {
namespace renderer {

/**
 * A pipeline property. Wraps GPU state to be set.
 */
class PipelineVariable {
};


/**
 * Pipeline uniform variable, which is a global value for all shader stages.
 */
template<typename T>
class Uniform : public PipelineVariable {
public:
	Uniform(const std::string &name, Program *program=nullptr);
	virtual ~Uniform();

	void set(const T &value);
	void set_program(Program *program);

	void apply();

protected:
	std::string name;
	T value;
	Program *program;
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
	// TODO

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
	 * Set the name of a uniform to a given value.
	 * e.g. set_uniform<vec3>("color", {0.0, 0.0, 1.0});
	 */
	template<typename T>
	void set_uniform(const std::string &name, const T &value) {
		Uniform<T> u{name, this->program};
		u.set(value);
		u.apply();
	}

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
