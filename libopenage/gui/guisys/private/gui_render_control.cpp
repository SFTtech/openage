// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_render_control.h"

namespace qtsdl {

RenderControl::RenderControl(QWindow *w)
	:
	QQuickRenderControl{}, //
	window{w} {
}

QWindow* RenderControl::renderWindow(QPoint *offset) {
	if (offset)
		*offset = QPoint{0, 0};

	return this->window;
}

} // namespace qtsdl
