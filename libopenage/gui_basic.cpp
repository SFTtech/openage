// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_basic.h"

namespace openage {
namespace gui {

GuiBasic::GuiBasic(SDL_Window *window, const std::string &source, qtsdl::GuiSingletonItemsInfo *singleton_items_info, const std::vector<std::string> &search_paths)
	:
	application{},
	render_updater{},
	renderer{window},
	game_logic_updater{},
	image_provider_by_filename{&render_updater, GuiGameSpecImageProvider::Type::ByFilename},
	image_provider_by_graphic_id{&render_updater, GuiGameSpecImageProvider::Type::ByGraphicId},
	image_provider_by_terrain_id{&render_updater, GuiGameSpecImageProvider::Type::ByTerrainId},
	engine{&renderer, {&image_provider_by_filename, &image_provider_by_graphic_id, &image_provider_by_terrain_id}, singleton_items_info},
	subtree{&renderer, &game_logic_updater, &engine, source, search_paths},
	input{&renderer, &game_logic_updater} {
}

GuiBasic::~GuiBasic() {
}

void GuiBasic::process_events() {
	this->game_logic_updater.process_callbacks();
	this->application.processEvents();
}

bool GuiBasic::on_resize(coord::window new_size) {
	this->renderer.resize(new_size.x, new_size.y);
	return true;
}

bool GuiBasic::on_input(SDL_Event *event) {
	return !this->input.process(event);
}

bool GuiBasic::on_drawhud() {
	this->render_updater.process_callbacks();
	this->renderer.render();
	return true;
}

}} // namespace openage::gui
