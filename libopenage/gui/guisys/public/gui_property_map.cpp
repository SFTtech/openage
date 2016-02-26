// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_property_map.h"

#include <algorithm>
#include <typeinfo>
#include <cassert>

#include <QVariant>
#include <QRegExp>

#include <QCoreApplication>

#include "../link/gui_property_map_impl.h"

namespace qtsdl {

GuiPropertyMap::GuiPropertyMap()
	:
	impl{std::make_unique<GuiPropertyMapImpl>()} {

	assert(QCoreApplication::instance());
}

GuiPropertyMap::~GuiPropertyMap() {
}

namespace {
template<typename T>
T getter(const QObject &map, const char *name) {
	auto v = map.property(name);

	if (!v.isValid())
		return T();

	using namespace std::string_literals;

	if (!v.canConvert<T>())
		throw std::runtime_error("Can't interpret a property '"s + name + "' of type '"s + v.typeName() + "' as '"s + typeid(T).name() + "'."s);

	return v.value<T>();
}

template<typename T>
void setter(QObject &map, const char *name, T v) {
	map.setProperty(name, QVariant::fromValue(v));
}

template<>
void setter<const char*>(QObject &map, const char *name, const char *v) {
	map.setProperty(name, v);
}

std::vector<std::string> strings_to_vector(const QStringList &strings) {
	std::vector<std::string> result(strings.size());
	std::transform(std::begin(strings), std::end(strings), std::begin(result), [] (const QString &s) {return s.toStdString();});

	return result;
}

QStringList vector_to_strings(const std::vector<std::string> &v) {
	QStringList strings;
	std::transform(std::begin(v), std::end(v), std::back_inserter(strings), [] (const std::string &s) {return QString::fromStdString(s);});

	return strings;
}
}

template<>
bool GuiPropertyMap::getv<bool>(const char *name) const {
	return getter<bool>(*this->impl, name);
}

template<>
void GuiPropertyMap::setv<bool>(const char *name, bool v) {
	setter(*this->impl, name, v);
}

template<>
int GuiPropertyMap::getv<int>(const char *name) const {
	return getter<int>(*this->impl, name);
}

template<>
void GuiPropertyMap::setv<int>(const char *name, int v) {
	setter(*this->impl, name, v);
}

template<>
double GuiPropertyMap::getv<double>(const char *name) const {
	return getter<double>(*this->impl, name);
}

template<>
void GuiPropertyMap::setv<double>(const char *name, double v) {
	setter(*this->impl, name, v);
}

template<>
std::string GuiPropertyMap::getv<std::string>(const char *name) const {
	return getter<QString>(*this->impl, name).toStdString();
}

template<>
void GuiPropertyMap::setv<const std::string&>(const char *name, const std::string &v) {
	setter(*this->impl, name, QString::fromStdString(v));
}

template<>
void GuiPropertyMap::setv<const char*>(const char *name, const char *v) {
	setter(*this->impl, name, v);
}

template<>
std::vector<std::string> GuiPropertyMap::getv<std::vector<std::string>>(const char *name) const {
	return strings_to_vector(getter<QStringList>(*this->impl, name));
}

template<>
void GuiPropertyMap::setv<const std::vector<std::string>&>(const char *name, const std::vector<std::string> &v) {
	setter(*this->impl, name, vector_to_strings(v));
}

template<>
QString GuiPropertyMap::getv<QString>(const char *name) const {
	return getter<QString>(*this->impl, name);
}

template<>
void GuiPropertyMap::setv<const QString&>(const char *name, const QString &v) {
	setter(*this->impl, name, v);
}

template<>
QStringList GuiPropertyMap::getv<QStringList>(const char *name) const {
	return getter<QStringList>(*this->impl, name);
}

template<>
void GuiPropertyMap::setv<const QStringList&>(const char *name, const QStringList &v) {
	setter(*this->impl, name, v);
}

void GuiPropertyMap::setv(const char *name, const std::string &v) {
	this->setv<const std::string&>(name, v);
}

void GuiPropertyMap::setv(const std::string &name, const std::string &v) {
	this->setv<const std::string&>(name.c_str(), v);
}

void GuiPropertyMap::setv(const char *name, const std::vector<std::string> &v) {
	this->setv<const std::vector<std::string>&>(name, v);
}

void GuiPropertyMap::setv(const std::string &name, const std::vector<std::string> &v) {
	this->setv<const std::vector<std::string>&>(name.c_str(), v);
}

std::vector<std::string> GuiPropertyMap::get_csv(const char *name) const {
	return strings_to_vector(getter<QString>(*this->impl, name).split(QRegExp("\\s*,\\s*")));
}

void GuiPropertyMap::set_csv(const char *name, const std::vector<std::string> &v) {
	setter(*this->impl, name, vector_to_strings(v).join(", "));
}

} // namespace qtsdl
