// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "curve_record_replay_link.h"

#include <QtQml>

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterUncreatableType<CurveRecordReplayLink>("yay.sfttech.openage", 1, 0, "CurveRecordReplay", "CurveRecordReplay is an abstract uncreatable base");
const int registration_recorder = qmlRegisterType<CurveRecordLink>("yay.sfttech.openage", 1, 0, "CurveRecord");
const int registration_player = qmlRegisterType<CurveReplayLink>("yay.sfttech.openage", 1, 0, "CurveReplay");
}

CurveRecordReplayLink::CurveRecordReplayLink(QObject *parent)
	:
	GuiItemQObject{parent},
	GuiItemInterface{} {
	Q_UNUSED(registration);
}

QString CurveRecordReplayLink::get_file_name() const {
	return this->file_name;
}

void CurveRecordReplayLink::set_file_name(const QString &file_name) {
	static auto f = [] (curve::CurveRecordReplay *_this, const QString &file_name) {
		_this->set_file_name(file_name.toStdString());
	};
	this->s(f, this->file_name, file_name);
}

void CurveRecordReplayLink::on_core_adopted() {
	this->file_name = QString::fromStdString(unwrap(this)->get_file_name());
}

CurveRecordLink::CurveRecordLink(QObject *parent)
	:
	Inherits{parent} {
	Q_UNUSED(registration_recorder);
}

CurveReplayLink::CurveReplayLink(QObject *parent)
	:
	Inherits{parent} {
	Q_UNUSED(registration_player);
}

}} // namespace openage::gui
