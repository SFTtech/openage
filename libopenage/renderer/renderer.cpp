// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "renderer.h"


Renderer::Renderer() {

}

Renderer::~Renderer() {

}

void Renderer::render() {
	sort instructions;
	for (each instruction) {
		apply necessary shader/texture changes;
		render instruction;
	}
}

void add_instruction(task t) {
	// insertion sort?
	// place the new task according to its order?
	this->tasks.push_back(t);
}

void render() {

}

std::vector<task> instructions_sorted() {
	// stably sort by:
	// layer, texture, shader
}
