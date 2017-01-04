// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "gui_live_reloader.h"

#include <algorithm>
#include <unordered_set>

#include "../../link/gui_item.h"
#include "deferred_initial_constant_property_values.h"

namespace qtsdl {

namespace {
const int registration = qmlRegisterUncreatableType<GuiLiveReloaderAttachedPropertyProvider>("yay.sfttech.livereload", 1, 0, "LR", "LR is non-instantiable. It provides the 'LR.tag' attached property.");
const int registration_property = qmlRegisterType<GuiLiveReloaderAttachedProperty>();
}

GuiLiveReloaderAttachedProperty::GuiLiveReloaderAttachedProperty(QObject *object)
	:
	QObject{object} {
	Q_UNUSED(registration);
	Q_UNUSED(registration_property);

	if (!dynamic_cast<GuiItemBase*>(this->parent()))
		qFatal("Error in QML code: tried to set the 'LR.tag' to item with non-persistent type '%s'.", object->metaObject()->className());
}

GuiItemBase* GuiLiveReloaderAttachedProperty::get_attachee() const {
	auto attachee = dynamic_cast<GuiItemBase*>(this->parent());
	assert(attachee);

	return attachee;
}

QString GuiLiveReloaderAttachedProperty::get_tag() const {
	qWarning("Some QML code is enquiring about the '%s' 'LR.tag'. You must not rely on the tag value.", qUtf8Printable(this->tag));
	return this->tag;
}

void GuiLiveReloaderAttachedProperty::set_tag(const QString &tag) {
	if (this->get_attachee()->is_init_over())
		qCritical("Error in QML code: tried to change the '%s' 'LR.tag' to '%s'.", qUtf8Printable(this->tag), qUtf8Printable(tag));
	else
		this->tag = tag;
}

QString GuiLiveReloaderAttachedProperty::get_tag_for_init() const {
	return this->tag;
}

GuiLiveReloaderAttachedProperty* GuiLiveReloaderAttachedPropertyProvider::qmlAttachedProperties(QObject *attachee) {
	return new GuiLiveReloaderAttachedProperty(attachee);
}

void GuiLiveReloader::init_persistent_items(const QList<GuiLiveReloaderAttachedProperty*> &items) {
	std::unordered_set<QString> stored_tags;
	std::transform(std::begin(this->preservable), std::end(this->preservable), std::inserter(stored_tags, std::end(stored_tags)), [] (const TagToPreservableMap::value_type &pair) {return pair.first;});

	for (auto ap : items) {
		auto obj = ap->get_attachee();
		auto tag = ap->get_tag_for_init();

		if (tag.isEmpty()) {
			qFatal("Error in QML code: an item with underlying type of '%s' has no 'LR.tag' property.", dynamic_cast<QObject*>(obj)->metaObject()->className());
		} else {
			auto found_it = this->preservable.find(tag);

			if (found_it != std::end(this->preservable)) {
				obj->adopt_core(found_it->second.get(), tag);
				obj->clear_static_properties();
			} else if (auto wrapped_core = obj->instantiate_core()) {
				obj->adopt_core(this->preservable.insert(found_it, std::make_pair(tag, std::move(wrapped_core)))->second.get(), tag);
			} else {
				qCritical("Error in QML code: trying to add an object to the GuiLiveReloader multiple times. Second time was with tag '%s'.", qUtf8Printable(tag));
			}

			stored_tags.erase(tag);
		}
	}

	for (auto ap : items)
		ap->get_attachee()->apply_static_properties();

	std::for_each(std::begin(stored_tags), std::end(stored_tags), [] (const QString &dangling) {
		qWarning("The '%s' 'LR.tag' hasn't found any recipients after GUI reload.", qUtf8Printable(dangling));
	});
}

} // namespace qtsdl
