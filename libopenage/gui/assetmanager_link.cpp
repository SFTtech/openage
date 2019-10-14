// Copyright 2015-2019 the openage authors. See copying.md for legal info.

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

AssetManagerLink::~AssetManagerLink() = default;


const util::Path &AssetManagerLink::get_asset_dir() const {
	return this->asset_dir;
}


void AssetManagerLink::set_asset_dir(const util::Path &asset_dir) {
	static auto f = [] (AssetManager *_this, const util::Path &dir) {
		_this->set_asset_dir(dir);
	};
	this->s(f, this->asset_dir, asset_dir);
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
