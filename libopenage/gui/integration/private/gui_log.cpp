// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "../../integration/private/gui_log.h"

#include <QString>

#include "../../../log/log.h"
#include "../../../log/message.h"

namespace openage {
namespace gui {

void gui_log(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	log::MessageBuilder builder{context.file, static_cast<unsigned>(context.line), context.function, [=] {
		switch (type) {
			case QtDebugMsg:
			return log::lvl::dbg;
			break;
			case QtWarningMsg:
			return log::lvl::warn;
			break;
			case QtCriticalMsg:
			return log::lvl::err;
			break;
			case QtFatalMsg:
			return log::lvl::crit;
			break;
			default:
			return log::lvl::warn;
			break;
		}
	}()};

	// TODO: maybe it's not UTF-8
	log::log(builder << msg.toUtf8().data());

	if (type == QtFatalMsg)
		abort();
}

}} // namespace openage::gui
