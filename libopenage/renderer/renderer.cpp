// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "renderer.h"

#include "context.h"
#include "program.h"
#include "texture.h"

namespace openage {
namespace renderer {

Renderer::Renderer(const std::shared_ptr<Context> &ctx)
	:
	context{ctx} {}

Renderer::~Renderer() {}

TaskState Renderer::add_task(const Task &task) {
	// sort by:
	// layer, texture, shader
	this->tasks.push(task);

	TaskState ret;
	return ret;
}


std::unique_ptr<Program> Renderer::add_program(const ProgramSource &source) {
	return this->context->register_program(source);
}


std::unique_ptr<Texture> Renderer::add_texture(const TextureData &data) {
	return this->context->register_texture(data);
}


void Renderer::screenshot(const std::string &filename) {
	this->context->screenshot(filename);
}


void Renderer::check_error() {
	this->context->check_error();
}


bool Renderer::on_resize(coord::window new_size) {
	this->context->resize(new_size);
	return true;
}


void Renderer::render() const {
	while (not this->tasks.empty()) {
		// Task = this->tasks.pop();
		// apply necessary shader/texture changes;
		// render instruction;
	}
}

}} // openage::renderer
