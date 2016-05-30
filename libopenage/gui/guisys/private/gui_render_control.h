// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QQuickRenderControl>

namespace qtsdl {

// TODO: separate GUI context and GUI sessions
class RenderControl : public QQuickRenderControl {
public:
	explicit RenderControl(QWindow *w);

	virtual QWindow* renderWindow(QPoint *offset) override;

private:
	QWindow *window;
};

} // namespace qtsdl
