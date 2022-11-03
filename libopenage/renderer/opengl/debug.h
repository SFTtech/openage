// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>
#include <QOpenGLDebugMessage>

namespace openage::renderer::opengl {

class GlDebugLogHandler : public QObject {
	Q_OBJECT
public:
	GlDebugLogHandler() = default;
	~GlDebugLogHandler() = default;

public slots:
	void log(const QOpenGLDebugMessage &debugMessage);
};

} // namespace openage::renderer::opengl