// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>
#include <QOpenGLDebugMessage>

QT_FORWARD_DECLARE_CLASS(QOpenGLDebugLogger)

namespace openage::renderer::opengl {

class GlDebugLogHandler : public QObject {
	Q_OBJECT

public:
	/**
	 * Create a new OpenGL debug logger.
	 */
	GlDebugLogHandler();
	~GlDebugLogHandler() = default;

	/**
	 * Start logging OpenGL debug messages.
	 *
	 * Logging requires a current OpenGL context. If no current context can
	 * be found, no log messages will be generated.
	 *
	 * You may call this method multiple times to reattach Qt's logger if
	 * the current context has changed.
	 */
	void start();

	/**
	 * Stop logging OpenGL debug messages.
	 */
	void stop();

public slots:
	/**
	 * Pass log messages to the openage logger.
	 *
	 * All messages are passed with DBG info level. This also means that the logger
	 * does not exit on OpenGL errors.
	 *
	 * @param debugMessage Qt's log message object.
	 */
	void log(const QOpenGLDebugMessage &debugMessage);

private:
	/**
	 * Logger for OpenGL debug messages
	 */
	std::shared_ptr<QOpenGLDebugLogger> logger;
};

} // namespace openage::renderer::opengl
