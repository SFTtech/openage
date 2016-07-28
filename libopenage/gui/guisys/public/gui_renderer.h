// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <GL/gl.h>

struct SDL_Window;

namespace qtsdl {

class GuiRendererImpl;

/**
 * Passes the native graphic context to Qt.
 */
class GuiRenderer {
public:
	// TODO: allow FBO variant
	explicit GuiRenderer(SDL_Window *window);
	~GuiRenderer();

	GLuint render();
	void resize(int w, int h);

private:
	friend class GuiRendererImpl;
	std::unique_ptr<GuiRendererImpl> impl;
};

} // namespace qtsdl
