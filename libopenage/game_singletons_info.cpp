// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "game_singletons_info.h"

namespace openage {
namespace gui {
GameSingletonsInfo::GameSingletonsInfo(Engine *engine, const std::string &data_dir)
	:
	GuiSingletonItemsInfo{},
	engine{engine},
	data_dir{data_dir} {
}

}} // namespace openage::gui
