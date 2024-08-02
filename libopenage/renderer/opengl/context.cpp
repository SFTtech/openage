// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#include <array>
#include <epoxy/gl.h>

#include "context.h"

#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QWindow>

#include "error/error.h"
#include "log/log.h"
#include "renderer/opengl/debug.h"


namespace openage::renderer::opengl {

/// The first element is the lowest version we need, last is highest version we support.
static constexpr std::array<std::pair<int, int>, 1> gl_versions = {{{3, 3}}}; // for now we don't need any higher versions

/// Finds out the supported graphics functions and OpenGL version of the device.
gl_context_spec GlContext::find_spec() {
	QSurfaceFormat test_format{};
	test_format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
	test_format.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);
	test_format.setDepthBufferSize(24);

	for (size_t i_ver = 0; i_ver < gl_versions.size(); ++i_ver) {
		QOpenGLContext test_context{};

		test_format.setMajorVersion(gl_versions[i_ver].first);
		test_format.setMinorVersion(gl_versions[i_ver].second);

		test_context.setFormat(test_format);
		test_context.create();

		if (!test_context.isValid()) {
			if (i_ver == 0) {
				throw Error(MSG(err) << "OpenGL version "
				                     << gl_versions[0].first << "." << gl_versions[0].second
				                     << " is not available. It is the minimal required version.");
			}

			test_format.setMajorVersion(gl_versions[i_ver - 1].first);
			test_format.setMinorVersion(gl_versions[i_ver - 1].second);
			break;
		}
	}

	QOpenGLContext test_context{};
	test_context.setFormat(test_format);
	test_context.create();
	if (!test_context.isValid()) {
		throw Error(MSG(err) << "Failed to create OpenGL context which previously succeeded. This should not happen!");
	}

	QOffscreenSurface test_surface{};
	test_surface.setFormat(test_format);
	test_surface.create();

	test_context.makeCurrent(&test_surface);

	gl_context_spec caps{};

	// Texture parameters
	GLint temp;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &temp);
	caps.max_texture_size = temp;
	// TODO maybe GL_MAX_TEXTURE_IMAGE_UNITS or maybe GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
	// lol opengl
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &temp);
	caps.max_texture_slots = temp;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &temp);
	caps.max_vertex_attributes = temp;
	glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &temp);
	caps.max_uniform_locations = temp;
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &temp);
	caps.max_uniform_buffer_bindings = temp;

	// OpenGL version
	glGetIntegerv(GL_MAJOR_VERSION, &caps.major_version);
	glGetIntegerv(GL_MINOR_VERSION, &caps.minor_version);

	return caps;
}

GlContext::GlContext(const std::shared_ptr<QWindow> &window,
                     bool debug) :
	window{window},
	log_handler{} {
	this->specs = find_spec();
	auto const &specs = this->specs;

	this->uniform_buffer_bindings = std::vector<bool>(specs.max_uniform_buffer_bindings);

	if (debug) {
		this->log_handler = std::make_shared<GlDebugLogHandler>();
	}

	this->gl_context = std::make_shared<QOpenGLContext>();
	this->gl_context->setFormat(this->window->requestedFormat());
	this->gl_context->create();
	if (!this->gl_context->isValid()) {
		throw Error(MSG(err) << "OpenGL context creation failed.");
	}

	this->gl_context->makeCurrent(window.get());

	if (debug) {
		// Log handler requires a current context, so we start it after associating
		// it with the window.
		this->log_handler->start();
	}

	// We still have to verify that our version of libepoxy supports this version of OpenGL.
	int epoxy_glv = specs.major_version * 10 + specs.minor_version;
	if (not epoxy_is_desktop_gl() or epoxy_gl_version() < epoxy_glv) {
		throw Error(MSG(err) << "The used version of libepoxy does not support OpenGL version "
		                     << specs.major_version << "." << specs.minor_version);
	}

	log::log(MSG(info) << "Created OpenGL context version " << specs.major_version << "." << specs.minor_version);

	// To quote the standard doc: 'The value gives a rough estimate of the
	// largest texture that the GL can handle'
	// -> wat?  anyways, we need at least 1024x1024.
	log::log(MSG(dbg) << "Maximum supported texture size: "
	                  << specs.max_texture_size);
	if (specs.max_texture_size < 1024) {
		throw Error(MSG(err) << "Maximum supported texture size is too small: "
		                     << specs.max_texture_size);
	}

	log::log(MSG(dbg) << "Maximum supported texture units: "
	                  << specs.max_texture_slots);
	if (specs.max_texture_slots < 2) {
		throw Error(MSG(err) << "Your GPU doesn't have enough texture units: "
		                     << specs.max_texture_slots);
	}
}

GlContext::GlContext(GlContext &&other) :
	gl_context(other.gl_context), specs(other.specs) {
	other.gl_context = nullptr;
}

GlContext &GlContext::operator=(GlContext &&other) {
	this->gl_context = other.gl_context;
	this->specs = other.specs;
	other.gl_context = nullptr;

	return *this;
}

std::shared_ptr<QOpenGLContext> GlContext::get_raw_context() const {
	return this->gl_context;
}

GLuint GlContext::get_default_framebuffer_id() {
	return this->gl_context->defaultFramebufferObject();
}

gl_context_spec GlContext::get_specs() const {
	return this->specs;
}

void GlContext::check_error() {
	GLenum error_state = glGetError();
	if (error_state != GL_NO_ERROR) {
		const char *msg = [=] {
			// generate error message
			switch (error_state) {
			case GL_INVALID_ENUM:
				// An unacceptable value is specified for an enumerated argument.
				// The offending command is ignored
				// and has no other side effect than to set the error flag.
				return "GL_INVALID_ENUM";
			case GL_INVALID_VALUE:
				// A numeric argument is out of range.
				// The offending command is ignored
				// and has no other side effect than to set the error flag.
				return "GL_INVALID_VALUE";
			case GL_INVALID_OPERATION:
				// The specified operation is not allowed in the current state.
				// The offending command is ignored
				// and has no other side effect than to set the error flag.
				return "GL_INVALID_OPERATION";
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				// The framebuffer object is not complete. The offending command
				// is ignored and has no other side effect than to set the error flag.
				return "GL_INVALID_FRAMEBUFFER_OPERATION";
			case GL_OUT_OF_MEMORY:
				// There is not enough memory left to execute the command.
				// The state of the GL is undefined,
				// except for the state of the error flags,
				// after this error is recorded.
				return "GL_OUT_OF_MEMORY";
			case GL_STACK_UNDERFLOW:
				// An attempt has been made to perform an operation that would
				// cause an internal stack to underflow.
				return "GL_STACK_UNDERFLOW";
			case GL_STACK_OVERFLOW:
				// An attempt has been made to perform an operation that would
				// cause an internal stack to overflow.
				return "GL_STACK_OVERFLOW";
			default:
				// unknown error state
				return "unknown error";
			}
		}();

		throw Error(
			MSG(err) << "An OpenGL error has occured.\n\t"
					 << "(" << error_state << "): " << msg);
	}
}

void GlContext::set_vsync(bool on) {
	if (on) {
		this->gl_context->format().setSwapInterval(1);
	}
	else {
		this->gl_context->format().setSwapInterval(0);
	}
}


const std::weak_ptr<GlShaderProgram> &GlContext::get_current_program() const {
	return this->last_program;
}


void GlContext::set_current_program(const std::shared_ptr<GlShaderProgram> &prog) {
	this->last_program = prog;
}

size_t GlContext::get_uniform_buffer_binding() {
	for (size_t i = 1; i < this->specs.max_uniform_buffer_bindings; ++i) {
		if (not this->uniform_buffer_bindings[i]) {
			this->uniform_buffer_bindings[i] = true;
			return i;
		}
	}

	throw Error(MSG(err) << "Cannot get free uniform buffer binding point: "
	                     << "No free uniform buffer binding points available.");
}

void GlContext::free_uniform_buffer_binding(size_t binding_point) {
	if (binding_point >= this->specs.max_uniform_buffer_bindings) [[unlikely]] {
		throw Error(MSG(err) << "Cannot free invalid uniform buffer binding point: " << binding_point);
	}
	this->uniform_buffer_bindings[binding_point] = false;
}


} // namespace openage::renderer::opengl
