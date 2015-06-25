// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "context.h"

#include "../config.h"
#include "../log/log.h"
#include "../util/error.h"

#if WITH_OPENGL
#include "opengl/context.h"
#endif

#if WITH_VULKAN
#include "vulkan/context.h"
#endif


namespace openage {
namespace renderer {

Context::Context() {}
Context::~Context() {}

std::unique_ptr<Context> Context::generate(context_type t) {
	context_type ctx_requested = t;

	if (t == context_type::autodetect) {
		// priority: vulkan > opengl
		if (WITH_VULKAN) {
			ctx_requested = context_type::vulkan;
		}
		else if (WITH_OPENGL) {
			ctx_requested = context_type::opengl;
		}
		else {
			throw util::Error(MSG(err) << "No render context available!");
		}
	}

	if (ctx_requested == context_type::opengl) {
		if (not WITH_OPENGL) {
			throw util::Error(MSG(err) << "OpenGL support not enabled!");
		}
#if WITH_OPENGL
		log::log(MSG(dbg) << "Using OpenGL context...");
		return std::make_unique<opengl::Context>();
#endif
	}
	else if (ctx_requested == context_type::vulkan) {
		if (not WITH_VULKAN) {
			throw util::Error(MSG(err) << "Vulkan support not enabled!");
		}
#if WITH_VULKAN
		log::log(MSG(dbg) << "Using Vulkan context...");
		return std::make_unique<vulkan::Context>();
#endif
	}
	else {
		throw util::Error(MSG(err) << "Unknown context type requested!");
	}

	throw util::Error(MSG(err) << "Context creation dead code reached! Veeery bad.");
}

}} // namespace openage::renderer
