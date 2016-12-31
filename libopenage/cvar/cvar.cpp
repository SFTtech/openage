// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#include "cvar.h"

#include "../log/log.h"


namespace openage {
namespace cvar {

bool CVarManager::create(const std::string &name,
                         const std::pair<get_func, set_func> &accessors) {
	auto it = this->store.find(name);
	if (it == this->store.end()) {
		this->store[name] = accessors;
		return true;
	}
	return false;
}


std::string CVarManager::get(const std::string &name) const {
	auto it = this->store.find(name);
	if (it != this->store.end()) {
		return it->second.first();
	}
	return "";
}


void CVarManager::set(const std::string &name, const std::string &value) const {
	auto it = store.find(name);
	if (it != store.end()) {
		it->second.second(value);
	}
}


std::string CVarManager::find_main_config() const {
	return find_main_config_file.call();
}


void CVarManager::load_config(const std::string &path) {
	load_config_file.call(path, this);
}


void CVarManager::load_main_config() {
	auto filename = this->find_main_config();
	log::log(INFO << "loading configuration files...");
	this->load_config(filename);
}


pyinterface::PyIfFunc<std::string> find_main_config_file;
pyinterface::PyIfFunc<void, std::string, CVarManager*> load_config_file;

}} // openage::cvar
