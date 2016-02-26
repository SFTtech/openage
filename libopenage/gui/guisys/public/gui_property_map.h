// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace qtsdl {

class GuiPropertyMapImpl;

struct GuiPropertyMap {
	GuiPropertyMap();
	~GuiPropertyMap();

	template<typename T>
	T getv(const char *name) const;

	template<typename T>
	T getv(const std::string &name) const {
		return this->getv<T>(name.c_str());
	}

	std::vector<std::string> get_csv(const char *name) const;

	std::vector<std::string> get_csv(const std::string &name) const {
		return this->get_csv(name.c_str());
	}

	template<typename T>
	void setv(const char *name, T v);

	template<typename T>
	void setv(const std::string &name, T v) {
		this->setv<T>(name.c_str(), v);
	}

	void set_csv(const char *name, const std::vector<std::string> &v);

	void set_csv(const std::string &name, const std::vector<std::string> &v) {
		this->set_csv(name.c_str(), v);
	}

	void setv(const char *name, const std::string &v);
	void setv(const std::string &name, const std::string &v);

	void setv(const char *name, const std::vector<std::string> &v);
	void setv(const std::string &name, const std::vector<std::string> &v);

	std::unique_ptr<GuiPropertyMapImpl> impl;
};

template<>
bool GuiPropertyMap::getv<bool>(const char *name) const;

template<>
void GuiPropertyMap::setv<bool>(const char *name, bool v);

template<>
int GuiPropertyMap::getv<int>(const char *name) const;

template<>
void GuiPropertyMap::setv<int>(const char *name, int v);

template<>
double GuiPropertyMap::getv<double>(const char *name) const;

template<>
void GuiPropertyMap::setv<double>(const char *name, double v);

template<>
std::string GuiPropertyMap::getv<std::string>(const char *name) const;

template<>
void GuiPropertyMap::setv(const char *name, const std::string &v);

template<>
void GuiPropertyMap::setv<const char*>(const char *name, const char *v);

template<>
std::vector<std::string> GuiPropertyMap::getv<std::vector<std::string>>(const char *name) const;

template<>
void GuiPropertyMap::setv<const std::vector<std::string>&>(const char *name, const std::vector<std::string> &v);

} // namespace qtsdl
