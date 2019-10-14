// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "generator_link.h"

#include <QtQml>

#include "guisys/link/gui_property_map_impl.h"

namespace openage::gui {

namespace
{
const int registration = qmlRegisterType<GeneratorLink>("yay.sfttech.openage", 1, 0, "GeneratorParameters");
}

GeneratorLink::GeneratorLink(QObject *parent)
	:
	GuiListModel{parent},
	GuiItemListModel{this} {
	Q_UNUSED(registration);
}

GeneratorLink::~GeneratorLink() = default;

} // namespace openage::gui
