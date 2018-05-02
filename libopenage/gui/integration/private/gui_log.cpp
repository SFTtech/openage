// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "../../integration/private/gui_log.h"

#include <QString>

#include "../../../log/log.h"

namespace openage {
namespace gui {

void gui_log(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	log::level msg_lvl;

	switch (type) {
	case QtDebugMsg:
		msg_lvl = log::level::dbg;
		break;
	case QtInfoMsg:
		msg_lvl = log::level::info;
		break;
	case QtWarningMsg:
		msg_lvl = log::level::warn;
		break;
	case QtCriticalMsg:
		msg_lvl = log::level::err;
		break;
	case QtFatalMsg:
		msg_lvl = log::level::crit;
		break;
	default:
		msg_lvl = log::level::warn;
		break;
	}

	log::MessageBuilder builder{
		context.file != nullptr ? context.file : "<unknown file>",
		static_cast<unsigned>(context.line),
	        context.function != nullptr ? context.function : "<unknown function>",
	        msg_lvl
	};

	// TODO: maybe it's not UTF-8
	// TODO: Qt should become a LogSource
	log::log(builder << msg.toUtf8().data());

	if (type == QtFatalMsg)
		abort();
}

}} // namespace openage::gui
