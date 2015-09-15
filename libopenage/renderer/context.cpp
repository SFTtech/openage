// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "context.h"

#include "../config.h"
#include "../log/log.h"
#include "../error/error.h"

#if WITH_OPENGL
#include "opengl/context.h"
#endif

#if WITH_VULKAN
#include "vulkan/context.h"
#endif


namespace openage {
namespace renderer {

Context::Context(context_type type)
	:
	type{type},
	pipeline{nullptr} {
}

std::unique_ptr<Context> Context::generate(context_type t) {
	context_type ctx_requested = t;

	if (t == context_type::autodetect) {
		// priority: vulkan > opengl
		// TODO: could use some boosting, that autodetection is kinda lame.

		if (WITH_VULKAN) {
			ctx_requested = context_type::vulkan;
		}
		else if (WITH_OPENGL) {
			ctx_requested = context_type::opengl;
		}
		else {
			throw Error{MSG(err) << "No render context available!"};
		}
	}

	if (ctx_requested == context_type::opengl) {
		if (not WITH_OPENGL) {
			throw Error{MSG(err) << "OpenGL support not enabled!"};
		}
#if WITH_OPENGL
		log::log(MSG(dbg) << "Using OpenGL context...");
		return std::make_unique<opengl::Context>();
#endif
	}
	else if (ctx_requested == context_type::vulkan) {
		if (not WITH_VULKAN) {
			throw Error{MSG(err) << "Vulkan support not enabled!"};
		}
#if WITH_VULKAN
		log::log(MSG(dbg) << "Using Vulkan context...");
		return std::make_unique<vulkan::Context>();
#endif
	}
	else {
		throw Error{MSG(err) << "Unknown context type requested!"};
	}

	throw Error{MSG(err) << "No context was created! Veeery bad."};
	return nullptr;
}


void Context::resize(const coord::window &new_size) {
	this->canvas_size = new_size;
	this->resize_canvas(this->canvas_size);
}


}} // namespace openage::renderer
