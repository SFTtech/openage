// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../assetmanager.h"

#include "guisys/link/gui_item.h"

namespace openage {
namespace gui {
class AssetManagerLink;
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

public:
	explicit AssetManagerLink(QObject *parent=nullptr);
	virtual ~AssetManagerLink();

	QString get_data_dir() const;
	void set_data_dir(const QString &data_dir);

private:
	QString data_dir;
};

}} // namespace openage::gui
