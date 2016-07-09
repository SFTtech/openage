// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "input_context_recorder_player_link.h"

#include <QtQml>

namespace openage {
namespace gui {

namespace {
const int registration_recorder = qmlRegisterType<InputContextRecorderLink>("yay.sfttech.openage", 1, 0, "InputContextRecorder");
const int registration_player = qmlRegisterType<InputContextPlayerLink>("yay.sfttech.openage", 1, 0, "InputContextPlayer");
}

InputContextRecorderPlayerLink::InputContextRecorderPlayerLink(QObject *parent)
	:
	GuiItemQObject{parent},
	GuiItemInterface{} {
}

QString InputContextRecorderPlayerLink::get_file_name() const {
	return this->file_name;
}

void InputContextRecorderPlayerLink::set_file_name(const QString &file_name) {
	static auto f = [] (input::InputContextRecorderPlayer *_this, const QString &file_name) {
		_this->set_file_name(file_name.toStdString());
	};
	this->s(f, this->file_name, file_name);
}

void InputContextRecorderPlayerLink::on_core_adopted() {
	this->file_name = QString::fromStdString(unwrap(this)->get_file_name());
}

InputContextRecorderLink::InputContextRecorderLink(QObject *parent)
	:
	Inherits{parent} {
	Q_UNUSED(registration_recorder);
}

InputContextPlayerLink::InputContextPlayerLink(QObject *parent)
	:
	Inherits{parent} {
	Q_UNUSED(registration_player);
}

}} // namespace openage::gui
