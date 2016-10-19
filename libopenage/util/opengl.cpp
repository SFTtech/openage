// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#include "opengl.h"

#include <epoxy/gl.h>

#include "../error/error.h"

namespace openage {
namespace util {

void gl_check_error() {
	int glerrorstate = 0;

	glerrorstate = glGetError();
	if (glerrorstate != GL_NO_ERROR) {

		const char *errormsg;

		//generate error message
		switch (glerrorstate) {
		case GL_INVALID_ENUM:
			// An unacceptable value is specified for an enumerated argument.
			// The offending command is ignored
			// and has no other side effect than to set the error flag.
			errormsg = "invalid enum passed to opengl call";
			break;
		case GL_INVALID_VALUE:
			// A numeric argument is out of range.
			// The offending command is ignored
			// and has no other side effect than to set the error flag.
			errormsg = "invalid value passed to opengl call";
			break;
		case GL_INVALID_OPERATION:
			// The specified operation is not allowed in the current state.
			// The offending command is ignored
			// and has no other side effect than to set the error flag.
			errormsg = "invalid operation performed during some state";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			// The framebuffer object is not complete. The offending command
			// is ignored and has no other side effect than to set the error flag.
			errormsg = "invalid framebuffer operation";
			break;
		case GL_OUT_OF_MEMORY:
			// There is not enough memory left to execute the command.
			// The state of the GL is undefined,
			// except for the state of the error flags,
			// after this error is recorded.
			errormsg = "out of memory, wtf?";
			break;
		case GL_STACK_UNDERFLOW:
			// An attempt has been made to perform an operation that would
			// cause an internal stack to underflow.
			errormsg = "stack underflow";
			break;
		case GL_STACK_OVERFLOW:
			// An attempt has been made to perform an operation that would
			// cause an internal stack to overflow.
			errormsg = "stack overflow";
			break;
		default:
			// unknown error state
			errormsg = "unknown error";
		}
		throw Error(MSG(err) <<
			"OpenGL error state after running draw method: " << glerrorstate << "\n"
			"\t" << errormsg << "\n"
			<< "Run the game with --gl-debug to get more information: './run game --gl-debug'.");
	}
}

}} // openage::util
