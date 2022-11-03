// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "debug.h"

#include "log/log.h"


namespace openage::renderer::opengl {

void GlDebugLogHandler::log(const QOpenGLDebugMessage &debugMessage) {
	log::log(MSG(dbg) << debugMessage.message().toStdString());
}

} // namespace openage::renderer::opengl