// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#include "engine_info.h"

namespace openage {
namespace gui {

EngineQMLInfo::EngineQMLInfo(LegacyEngine *engine,
                             const util::Path &asset_dir) :
	engine{engine},
	asset_dir{asset_dir} {}

} // namespace gui
} // namespace openage
