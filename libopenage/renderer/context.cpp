// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "context.h"

#include "../config.h"
#include "../log/log.h"
#include "../util/error.h"

#if WITH_OPENGL
#include "gl/context.h"
#endif

#if WITH_VULKAN
#include "vulkan/context.h"
#endif


namespace openage {
namespace renderer {

Context::Context() {}
Context::~Context() {}

std::unique_ptr<Context> Context::generate(context_type t) {
	if (t == context_type::opengl and not WITH_OPENGL) {
		throw util::Error(MSG(err) << "OpenGL support not enabled!");
	}
	else if (t == context_type::vulkan and not WITH_VULKAN) {
		throw util::Error(MSG(err) << "Vulkan support not enabled!");
	}
	else if (t == context_type::autodetect) {
		// priority: vulkan > opengl
		if (WITH_VULKAN) {
#if WITH_VULKAN
			log::log(MSG(dbg) << "Using Vulkan context...");
			return std::make_unique<VulkanContext>();
#endif
		}
		else if (WITH_OPENGL) {
#if WITH_OPENGL
			log::log(MSG(dbg) << "Using OpenGL context...");
			return std::make_unique<GLContext>();
#endif
		}
		else {
			throw util::Error(MSG(err) << "No render context available!");
		}
	}
	else {
		throw util::Error(MSG(err) << "Unknown context type requested!");
	}

	throw util::Error(MSG(err) << "Context creation dead code reached! Veeery bad.");
}

}} // namespace openage::renderer
