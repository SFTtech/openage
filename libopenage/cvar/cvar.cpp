// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#include "cvar.h"
#include "../engine.h"

namespace openage {
namespace cvar {

bool CVarManager::create(const std::string &name,  const std::pair<get_func, set_func> &accessors) {
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
	return CVarManager::find_main_config_file.call();
}

void CVarManager::load_config(const std::string &path) {
	CVarManager::load_config_file.call(path, this);
}

void CVarManager::load_main_config() {
	this->load_config(this->find_main_config());
}

CVarManager &CVarManager::get() {
	return Engine::get().get_cvar_manager();
}

pyinterface::PyIfFunc<std::string> CVarManager::find_main_config_file;
pyinterface::PyIfFunc<void, std::string, CVarManager*> CVarManager::load_config_file;
}
}
