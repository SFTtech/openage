// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#include "qml_info.h"

namespace openage {
namespace renderer {
namespace gui {

QMLInfo::QMLInfo(gamestate::GameSimulation *engine, const util::Path &asset_dir) :
	engine{engine},
	asset_dir{asset_dir} {}

} // namespace gui
} // namespace renderer
} // namespace openage
