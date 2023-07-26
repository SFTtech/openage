// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "debug.h"

#include <QOpenGLDebugLogger>

#include "log/log.h"
#include "log/message.h"


namespace openage::renderer::opengl {

GlDebugLogHandler::GlDebugLogHandler() :
	logger{std::make_shared<QOpenGLDebugLogger>()} {
}

void GlDebugLogHandler::start() {
	this->logger->initialize();
	QObject::connect(this->logger.get(),
	                 &QOpenGLDebugLogger::messageLogged,
	                 this,
	                 &GlDebugLogHandler::log);
	this->logger->startLogging();
}

void GlDebugLogHandler::stop() {
	this->logger->stopLogging();
}

void GlDebugLogHandler::log(const QOpenGLDebugMessage &debugMessage) {
	log::log(MSG(dbg) << debugMessage.message().toStdString());
}

} // namespace openage::renderer::opengl
