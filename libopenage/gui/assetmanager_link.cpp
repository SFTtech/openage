// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "assetmanager_link.h"

#include <QtQml>

#include "engine_link.h"

namespace openage {

class Engine;

namespace gui {

namespace {
const int registration = qmlRegisterType<AssetManagerLink>("yay.sfttech.openage", 1, 0, "AssetManager");
}

AssetManagerLink::AssetManagerLink(QObject *parent)
	:
	GuiItemQObject{parent},
	GuiItem{this} {
	Q_UNUSED(registration);
}

AssetManagerLink::~AssetManagerLink() {
}

QString AssetManagerLink::get_data_dir() const {
	return this->data_dir;
}

void AssetManagerLink::set_data_dir(const QString &data_dir) {
	static auto f = [] (AssetManager *_this, const QString &data_dir) {
		_this->set_data_dir_string(data_dir.toStdString());
	};
	this->s(f, this->data_dir, data_dir);
}


EngineLink *AssetManagerLink::get_engine() const {
	return this->engine;
}

void AssetManagerLink::set_engine(EngineLink *engine_link) {
	static auto f = [] (AssetManager *_this, Engine *engine) {
		_this->set_engine(engine);
	};
	this->s(f, this->engine, engine_link);
}


}} // namespace openage::gui
