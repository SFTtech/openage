// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "gui_item.h"
#include "gui_property_map_impl.h"
#include "gui_list_model.h"

namespace qtsdl {

/**
 * A shell object fir cores inherited from GuiPropertyMap
 *
 * Core can use a key-value interface while in QML it looks like a ListModel.
 */
template<typename T>
class GuiItemListModel : public GuiItem<T> {
public:
	explicit GuiItemListModel(GuiItemBase *item_base)
		:
		GuiItem<T>{item_base} {

		item_base->on_core_adopted_func = std::bind(&GuiItemListModel::on_core_adopted, this);
	}

private:
	void on_core_adopted() {
		this->establish_from_gui_propagation();
		this->do_initial_to_gui_propagation();
		this->establish_to_gui_propagation();
	}

	void establish_from_gui_propagation() {
		auto _this = checked_static_cast<T*>(this);

		QObject::connect(_this, &GuiListModel::changed_from_gui, [_this] (const QByteArray &name, const QVariant &value) {
			emit _this->game_logic_caller.in_game_logic_thread_blocking([&] {
				unwrap(_this)->impl->setProperty(name, value);
			});
		});
	}

	void establish_to_gui_propagation() {
		auto _this = checked_static_cast<T*>(this);

		auto properties = unwrap(_this)->impl.get();
		QObject::connect(properties, &GuiPropertyMapImpl::property_changed, _this, &GuiListModel::on_property_changed);
	}

	void do_initial_to_gui_propagation() {
		auto _this = checked_static_cast<T*>(this);

		auto properties = unwrap(_this)->impl.get();
		auto property_names = properties->dynamicPropertyNames();

		std::vector<std::tuple<QByteArray, QVariant>> values;
		values.reserve(property_names.size());

		std::transform(std::begin(property_names), std::end(property_names), std::back_inserter(values), [properties] (const QByteArray &name) {
			return std::make_tuple(name, properties->property(name));
		});

		_this->set(values);
	}
};

} // namespace qtsdl
