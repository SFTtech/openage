// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "engine_info.h"

namespace openage {
namespace gui {

EngineQMLInfo::EngineQMLInfo(Engine *engine,
                             const util::Path &asset_dir)
	:
	engine{engine},
	asset_dir{asset_dir} {}

}} // namespace openage::gui
