// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "gui/guisys/public/gui_singleton_items_info.h"
#include "util/path.h"

namespace openage {

namespace renderer {
namespace gui {

class QMLInfo : public qtsdl::GuiSingletonItemsInfo {
public:
	QMLInfo(const util::Path &asset_dir);

	/**
	 * Search path for finding assets n stuff.
	 */
	util::Path asset_dir;
};


} // namespace gui
} // namespace renderer
} // namespace openage
