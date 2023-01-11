// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "assets/assetmanager.h"
#include "gui/guisys/link/gui_item.h"
#include "util/path.h"


namespace openage {
namespace renderer::gui {
class AssetManagerLink;
class EngineLink;
} // namespace renderer::gui
} // namespace openage

namespace qtsdl {
template <>
struct Wrap<openage::AssetManager> {
	using Type = openage::renderer::gui::AssetManagerLink;
};

template <>
struct Unwrap<openage::renderer::gui::AssetManagerLink> {
	using Type = openage::AssetManager;
};

} // namespace qtsdl


namespace openage {
namespace renderer {
namespace gui {

class AssetManagerLink : public qtsdl::GuiItemQObject
	, public qtsdl::GuiItem<AssetManagerLink> {
	Q_OBJECT

	Q_PROPERTY(openage::util::Path assetDir READ get_asset_dir WRITE set_asset_dir)
	Q_MOC_INCLUDE("renderer/gui/engine_link.h")
	Q_PROPERTY(EngineLink *engine READ get_engine WRITE set_engine)

public:
	explicit AssetManagerLink(QObject *parent = nullptr);
	virtual ~AssetManagerLink();

	const util::Path &get_asset_dir() const;
	void set_asset_dir(const util::Path &data_dir);

	EngineLink *get_engine() const;
	void set_engine(EngineLink *engine);

private:
	util::Path asset_dir;
	EngineLink *engine;
};

} // namespace gui
} // namespace renderer
} // namespace openage