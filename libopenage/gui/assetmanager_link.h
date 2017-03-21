// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../assetmanager.h"
#include "../util/path.h"
#include "guisys/link/gui_item.h"


namespace openage {
namespace gui {
class AssetManagerLink;
class EngineLink;
}} // openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::AssetManager> {
	using Type = openage::gui::AssetManagerLink;
};

template<>
struct Unwrap<openage::gui::AssetManagerLink> {
	using Type = openage::AssetManager;
};

} // qtsdl


namespace openage {
namespace gui {

class AssetManagerLink : public qtsdl::GuiItemQObject, public qtsdl::GuiItem<AssetManagerLink> {
	Q_OBJECT

	Q_PROPERTY(openage::util::Path assetDir READ get_asset_dir WRITE set_asset_dir)
	Q_PROPERTY(openage::gui::EngineLink *engine READ get_engine WRITE set_engine)

public:
	explicit AssetManagerLink(QObject *parent=nullptr);
	virtual ~AssetManagerLink();

	const util::Path &get_asset_dir() const;
	void set_asset_dir(const util::Path &data_dir);

	EngineLink *get_engine() const;
	void set_engine(EngineLink *engine);

private:
	util::Path asset_dir;
	EngineLink *engine;
};

}} // namespace openage::gui
