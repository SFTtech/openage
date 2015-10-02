// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_CONTEXT_STATE_H_
#define OPENAGE_RENDERER_CONTEXT_STATE_H_

namespace openage {
namespace renderer {

class Program;
class VertexState;

class ContextState {
public:
	ContextState();
	~ContextState() = default;

	/**
	 * Active pipeline program.
	 */
	Program *program;

	/**
	 * Active vertex size, type and buffer configuration.
	 */
	VertexState *vertex_state;
};

}} // openage::renderer

#endif
