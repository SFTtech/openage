// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../gamestate/generator.h"

#include "guisys/link/gui_list_model.h"
#include "guisys/link/gui_item_list_model.h"

namespace openage {
class Generator;
namespace gui {
class GeneratorLink;
}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::Generator> {
	using Type = openage::gui::GeneratorLink;
};

template<>
struct Unwrap<openage::gui::GeneratorLink> {
	using Type = openage::Generator;
};

}

namespace openage {
namespace gui {

class GeneratorLink : public qtsdl::GuiListModel, public qtsdl::GuiItemListModel<GeneratorLink> {
	Q_OBJECT

public:
	GeneratorLink(QObject *parent=nullptr);
	virtual ~GeneratorLink();
};

}} // namespace openage::gui
