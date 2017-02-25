// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "cvar.h"

#include <tuple>

#include "../log/log.h"
#include "../error/error.h"


namespace openage {
namespace cvar {

CVarManager::CVarManager(const util::Path &path)
	:
	path{path},
	config_loading{} {
}


CVarManager::~CVarManager() {
	for (const auto &orphan : this->orphaned)
		log::log(MSG(warn) << "leftover unused cvar: '" << orphan.first << "'");
}


bool CVarManager::create(const std::string &name,
                         const std::pair<get_func, set_func> &accessors) {
	auto it = this->store.find(name);
	if (it == this->store.end()) {
		this->store[name] = accessors;

		auto orphan_it = std::find_if(std::begin(this->orphaned), std::begin(this->orphaned), [&name](const std::pair<std::string, std::string> &orphan) { return name == orphan.first; });

		if (orphan_it != std::end(this->orphaned)) {
			const auto orphan_value = orphan_it->second;
			this->orphaned.erase(orphan_it);

			std::get<set_func>(accessors)(orphan_value);
		}

		return true;
	}
	return false;
}


std::string CVarManager::get(const std::string &name) const {
	std::lock_guard<std::mutex> lock(this->store_mutex);

	auto it = this->store.find(name);
	if (it != this->store.end()) {
		return it->second.first();
	}
	return "";
}


std::vector<std::string> CVarManager::get_names() const {
	std::vector<std::string> result;
	result.reserve(this->store.size());

	std::lock_guard<std::mutex> lock(this->store_mutex);

	for (const auto &opt : this->store) {
		result.emplace_back(std::get<const std::string>(opt));
	}

	return result;
}


void CVarManager::set(const std::string &name, const std::string &value) {
	set_func setter;

	{
		std::lock_guard<std::mutex> lock(this->store_mutex);

		auto it = store.find(name);
		if (it != store.end()) {
			setter = it->second.second;
		}
	}

	if (setter) {
		setter(value);

		std::for_each(std::begin(this->callbacks), std::end(this->callbacks), std::bind(&CVarChangedCallback::operator(), std::placeholders::_1, name));

		if (!this->config_loading) {
			pyx_config_file_set_option.call(this->path["keybinds.oac"], name, value);
		}
	} else {
		auto orphan_it = std::find_if(std::begin(this->orphaned), std::begin(this->orphaned), [&name](const std::pair<std::string, std::string> &orphan) { return name == orphan.first; });

		if (orphan_it != std::end(this->orphaned)) {
			orphan_it->second = value;
		} else {
			this->orphaned.emplace_back(name, value);
		}
	}
}


void CVarManager::add_callback(CVarChangedCallback &callback) {
	this->callbacks.push_back(&callback);
}


void CVarManager::replace_callback(CVarChangedCallback &former, CVarChangedCallback &current) {
	auto found_it = std::find(std::begin(this->callbacks), std::end(this->callbacks), &former);
	ENSURE(found_it != std::end(this->callbacks), "Inconsistency in CVarManager callbacks.");

	*found_it = &current;
}


void CVarManager::remove_callback(CVarChangedCallback &callback) {
	this->callbacks.erase(std::remove(std::begin(this->callbacks), std::end(this->callbacks), &callback), std::end(this->callbacks));
}


CVarChangedCallback::CVarChangedCallback()
	:
	cvar_manager{} {
}


CVarChangedCallback::CVarChangedCallback(CVarManager &cvar_manager, Handler handler)
	:
	cvar_manager{&cvar_manager},
	handler{handler} {

	this->cvar_manager->add_callback(*this);
}


CVarChangedCallback::~CVarChangedCallback() {
	if (this->cvar_manager)
		this->cvar_manager->remove_callback(*this);
}


CVarChangedCallback::CVarChangedCallback(CVarChangedCallback &&o)
	:
	cvar_manager{std::move(o.cvar_manager)},
	handler{std::move(o.handler)} {

	o.cvar_manager = nullptr;
	this->cvar_manager->replace_callback(o, *this);
}


CVarChangedCallback& CVarChangedCallback::operator=(CVarChangedCallback &&o) {
	this->cvar_manager = std::move(o.cvar_manager);
	this->handler = std::move(o.handler);
	o.cvar_manager = nullptr;
	this->cvar_manager->replace_callback(o, *this);
	return *this;
}


void CVarChangedCallback::operator()(const std::string &name) const {
	this->handler(name);
}


CVarManager* CVarChangedCallback::manager() const {
	return this->cvar_manager;
}


void CVarManager::load_config(const util::Path &path) {
	this->config_loading = true;
	pyx_load_config_file.call(this, path);
	this->config_loading = false;
}


void CVarManager::load_all() {
	log::log(INFO << "loading configuration files...");
	this->load_config(this->path["keybinds.oac"]);
}

pyinterface::PyIfFunc<void, CVarManager *, const util::Path &> pyx_load_config_file;
pyinterface::PyIfFunc<void, const util::Path &, std::string, std::string> pyx_config_file_set_option;

}} // openage::cvar
