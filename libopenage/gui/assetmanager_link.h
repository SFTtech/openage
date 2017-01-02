// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../assetmanager.h"

#include "guisys/link/gui_item.h"

namespace openage {
namespace gui {
class AssetManagerLink;
class EngineLink;
}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::AssetManager> {
	using Type = openage::gui::AssetManagerLink;
};

template<>
struct Unwrap<openage::gui::AssetManagerLink> {
	using Type = openage::AssetManager;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class AssetManagerLink : public qtsdl::GuiItemQObject, public qtsdl::GuiItem<AssetManagerLink> {
	Q_OBJECT

	Q_PROPERTY(QString dataDir READ get_data_dir WRITE set_data_dir)
	Q_PROPERTY(openage::gui::EngineLink *engine READ get_engine WRITE set_engine)

public:
	explicit AssetManagerLink(QObject *parent=nullptr);
	virtual ~AssetManagerLink();

	QString get_data_dir() const;
	void set_data_dir(const QString &data_dir);

	EngineLink *get_engine() const;
	void set_engine(EngineLink *engine);

private:
	QString data_dir;
	EngineLink *engine;
};

}} // namespace openage::gui
